#include <QDebug>
#include <QPointer>
#include <QString>
#include <QList>
#include <QStringList>
#include <QMap>

#include "pncore/corpus/Corpus.h"
#include "pncore/annotation/IntervalTier.h"
#include "pncore/annotation/AnnotationTierGroup.h"
#include "pncore/interfaces/praat/PraatTextGrid.h"
#include "pncore/datastore/CorpusRepository.h"
#include "pncore/datastore/AnnotationDatastore.h"
#include "pncore/datastore/FileDatastore.h"
#include "pncore/statistics/Measures.h"
#include "pncore/statistics/StatisticalSummary.h"
using namespace Praaline::Core;

#include "pnlib/asr/htk/HTKForcedAligner.h"
#include "pnlib/asr/syllabifier/SyllabifierEasy.h"
using namespace  Praaline::ASR;

#include "ExperimentUtterances.h"

ExperimentUtterances::ExperimentUtterances()
{

}

QString ExperimentUtterances::loadTranscriptions(QPointer<CorpusCommunication> com)
{
    QString ret;

    // Import transcriptions (and possibly phonetisation) from file
    QHash<QString, QStringList> importedTranscriptions;
    QString path = "/Users/george";

    QString line;
    QFile file(path + "/transcription.txt");
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) return "Error reading transcriptions file";
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    do {
        line = stream.readLine();
        if (line.startsWith("#")) continue;
        QStringList fields = line.split("\t");
        importedTranscriptions.insert(fields.at(0), fields);
    } while (!stream.atEnd());
    file.close();

    // Add transcription to the children of this communication
    if (!com) return "Error";
    foreach (QPointer<CorpusRecording> rec, com->recordings()) {
        if (!rec) continue;
        // Get utterance ID from metadata
        QString utteranceID = com->property("StimulusID").toString();
        QString annotationID = rec->ID();
        QString speakerID = com->property("SubjectID").toString();
        // Check whether a transcription for this utterance is contained in the imported transcriptions
        if (!importedTranscriptions.contains(utteranceID)) continue;
        // Find or create the corresponding annotation
        QPointer<CorpusAnnotation> annot = com->annotation(annotationID);
        if (!annot) {
            annot = new CorpusAnnotation(annotationID);
            com->addAnnotation(annot);
            com->save();
        }
        // Create a tier for importing the transcription
        QSharedPointer<IntervalTier> tier_trancsription(new IntervalTier("transcription", RealTime(0, 0), rec->duration()));
        // Pauses at beginning and end
        tier_trancsription->split(RealTime::fromMilliseconds(100));
        tier_trancsription->split(tier_trancsription->tMax() - RealTime::fromMilliseconds(100));
        if (tier_trancsription->count() < 3) continue;
        tier_trancsription->interval(0)->setText("_");
        tier_trancsription->interval(2)->setText("_");
        // Transcription
        tier_trancsription->interval(1)->setText(importedTranscriptions.value(utteranceID).at(1));
        tier_trancsription->interval(1)->setAttribute("phonetisation", importedTranscriptions.value(utteranceID).at(2));
        // Save tier
        com->repository()->annotations()->saveTier(annotationID, speakerID, tier_trancsription.data());
        ret.append(QString("%1\t%2\t%3").arg(com->ID()).arg(annotationID).arg(speakerID));
    }
    return ret;
}

QString ExperimentUtterances::align(QPointer<Praaline::Core::CorpusCommunication> com)
{
    QString ret;
    if (!com) return "Error";
    QPointer<CorpusRecording> rec = com->recordings().first();
    if (!rec) return "Error";
    QString annotationID = rec->ID();
    QString speakerID = com->property("SubjectID").toString();
    IntervalTier *tier_tokens = qobject_cast<IntervalTier *>
            (com->repository()->annotations()->getTier(annotationID, speakerID, "tok_min"));
    if (!tier_tokens) return "No tier tokens";
    QList<Interval *> list_phones;
    QString alignerOutput;
    HTKForcedAligner aligner;
    bool ok = aligner.alignUtterance(rec->filePath(), tier_tokens, list_phones, alignerOutput);
    ret.append(QString("%1\t%2\t%3").arg(annotationID).arg(speakerID).arg((ok) ? "OK" : "Error"));
    if (!ok) {
        ret.append("\n").append(alignerOutput);
    }

    if (ok) {
        IntervalTier *tier_phone = new IntervalTier("phone", list_phones);
        IntervalTier *tier_syll= SyllabifierEasy::syllabify(tier_phone);
        tier_syll->setName("syll");
        com->repository()->annotations()->saveTier(annotationID, speakerID, tier_tokens);
        com->repository()->annotations()->saveTier(annotationID, speakerID, tier_phone);
        com->repository()->annotations()->saveTier(annotationID, speakerID, tier_syll);
    }

    delete tier_tokens;
    qDeleteAll(list_phones);

    return ret;
}

QString ExperimentUtterances::syllabify(QPointer<Praaline::Core::CorpusCommunication> com)
{
    QString ret;
    if (!com) return "Error";
    QPointer<CorpusRecording> rec = com->recordings().first();
    if (!rec) return "Error";
    QString annotationID = rec->ID();
    QString speakerID = com->property("SubjectID").toString();

    IntervalTier *tier_phone = qobject_cast<IntervalTier *>
            (com->repository()->annotations()->getTier(annotationID, speakerID, "phone"));
    if (!tier_phone) return "No tier phone";
    IntervalTier *tier_syll = SyllabifierEasy::syllabify(tier_phone);
    tier_syll->setName("syll");
    com->repository()->annotations()->saveTier(annotationID, speakerID, tier_syll);
    return ret;
}

QString ExperimentUtterances::fixTiers(QPointer<Praaline::Core::CorpusCommunication> com)
{
    QString ret;
    if (!com) return "Error";
    QPointer<CorpusRecording> rec = com->recordings().first();
    if (!rec) return "Error";
    QString annotationID = rec->ID();
    QString speakerID = com->property("SubjectID").toString();
    QStringList tierNames;
    tierNames << "phone" << "syll" << "tok_min" << "tok_mwu" << "prosodic_unit" << "sequence" << "ID";
    foreach (QString tierName, tierNames) {
        IntervalTier *tier = qobject_cast<IntervalTier *>
                (com->repository()->annotations()->getTier(annotationID, speakerID, tierName));
        if (!tier) continue;
        if (tier->count() < 1) continue;
        tier->moveTierEnd(rec->duration());
        com->repository()->annotations()->saveTier(annotationID, speakerID, tier);
        delete tier;
    }
    ret.append(com->ID());
    return ret;
}

QString ExperimentUtterances::fixTranscription(QPointer<Praaline::Core::CorpusCommunication> com)
{
    QString ret;
    if (!com) return "Error";
    QPointer<CorpusRecording> rec = com->recordings().first();
    if (!rec) return "Error";
    QString annotationID = rec->ID();
    QString speakerID = com->property("SubjectID").toString();
    IntervalTier *tier_transcription = qobject_cast<IntervalTier *>
            (com->repository()->annotations()->getTier(annotationID, speakerID, "transcription"));
    if (!tier_transcription) return "Error: no tier transcription";
    IntervalTier *tier_tokens = qobject_cast<IntervalTier *>
            (com->repository()->annotations()->getTier(annotationID, speakerID, "tok_min"));
    if (!tier_tokens) return "Error: no tier tokens";
    QList<RealTime> boundaries;
    boundaries << tier_tokens->first()->tMin() << tier_tokens->first()->tMax();
    boundaries << tier_tokens->last()->tMin();
    bool ok = tier_transcription->realignIntervals(0, boundaries);
    tier_transcription->moveTierEnd(tier_tokens->tMax());
    com->repository()->annotations()->saveTier(annotationID, speakerID, tier_transcription);
    delete tier_tokens;
    delete tier_transcription;
    ret.append(com->ID());
    ret.append((ok) ? " OK" : " Error");
    return ret;
}

QString ExperimentUtterances::concatenate(QPointer<Praaline::Core::CorpusCommunication> com)
{
    QString ret;
    if (!com) return "Error";
    QPointer<Corpus> corpus = com->corpus();
    if (!corpus) return "Error";

    // Group communications that will be concatenated
    QMap<QString, QStringList> concatenationGroups;
    foreach (QPointer<CorpusCommunication> com, corpus->communications()) {
        if (!com) continue;
        QString groupID = com->property("SubjectID").toString();
        if (!concatenationGroups.contains(groupID))
            concatenationGroups.insert(groupID, QStringList() << com->ID());
        else
            concatenationGroups[groupID] << com->ID();
    }
    foreach (QString groupID, concatenationGroups.keys()) {
        // Hack
        if (groupID != "S0") continue;
        //
        QStringList comIDs = concatenationGroups.value(groupID);
        qSort(comIDs);
        QString commandSoxConcatenate = "sox ";
        QSharedPointer<AnnotationTierGroup> tierGroupConcatenated(new AnnotationTierGroup);
        QList<Interval *> intervals_IDs;
        QStringList tierNames; tierNames << "phone" << "syll" << "tok_min" << "prosodic_unit" << "sequence";
        QHash<QString, QList<Interval *> > intervals_annot;
        foreach (QString levelID, tierNames)
            intervals_annot.insert(levelID, QList<Interval *>());
        RealTime offset;
        foreach (QString comID, comIDs) {
            QPointer<CorpusCommunication> com = corpus->communication(comID);
            if (!com) continue;
            foreach (QPointer<CorpusRecording> rec, com->recordings()) {
                commandSoxConcatenate = commandSoxConcatenate.append(rec->filePath()).append(" ");
                // Get annotation for this recording
                RealTime maxDuration = rec->duration();
                QString annotationID = rec->ID();
                foreach (QString levelID, tierNames) {
                    IntervalTier *tier = qobject_cast<IntervalTier *>
                            (corpus->repository()->annotations()->getTier(annotationID, groupID, levelID));
                    if (!tier) continue;
                    tier->timeShift(offset);
                    intervals_annot[levelID] << tier->intervals();
                    // if (maxDuration < tier->duration()) maxDuration = tier->duration();
                }
                intervals_IDs << new Interval(offset, offset + maxDuration, rec->ID());
                offset = offset + maxDuration;
            }
        }
        QString basePathForConcatenatedMedia = corpus->repository()->files()->basePath();
        commandSoxConcatenate = commandSoxConcatenate.append(basePathForConcatenatedMedia + "/" + groupID + ".wav");
        ret.append(commandSoxConcatenate).append("\n");
        // Export textgrid
        foreach (QString levelID, tierNames) {
            IntervalTier *tier = new IntervalTier(levelID, intervals_annot[levelID]);
            tierGroupConcatenated->addTier(tier);
        }
        IntervalTier *tier_IDs = new IntervalTier("ID", intervals_IDs);
        tierGroupConcatenated->addTier(tier_IDs);
        PraatTextGrid::save(basePathForConcatenatedMedia + "/" + groupID + ".TextGrid", tierGroupConcatenated.data());
    }

    return ret;
}

QString ExperimentUtterances::createUnitTier(QPointer<Praaline::Core::CorpusCommunication> com)
{
    QString ret;
    // Import sequence trigrams
    QHash<QString, QStringList> trigrams;
    QString path = "/mnt/hgfs/Dropbox/MIS_Phradico/Experiences/03_Production-prosodie-relations-de-discours/Production";

    QString line;
    QFile file(path + "/sequences_trigrams.txt");
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) return "Error reading trigrams file";
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    do {
        line = stream.readLine();
        if (line.startsWith("#")) continue;
        QStringList fields = line.split("\t");
        trigrams.insert(fields.at(0), fields);
    } while (!stream.atEnd());
    file.close();

    if (!com) return "Error";
    QPointer<Corpus> corpus = com->corpus();
    if (!corpus) return "Error";
    foreach (QPointer<CorpusCommunication> com, corpus->communications()) {
        if (!com) continue;
        QPointer<CorpusRecording> rec = com->recordings().first();
        if (!rec) continue;
        QString annotationID = rec->ID();
        QString speakerID = com->property("SubjectID").toString();

        if (speakerID != "SX2") continue;

        IntervalTier *tier_tokens = qobject_cast<IntervalTier *>
                (com->repository()->annotations()->getTier(annotationID, speakerID, "tok_min"));
        if (!tier_tokens) return "No tier tokens";
        IntervalTier *tier_units = new IntervalTier(tier_tokens, "sequence", false);
        QString stimulusID = com->property("StimulusID").toString();
        QString tokenBefore = trigrams[stimulusID].at(1);
        QString tokenTarget = trigrams[stimulusID].at(2);
        QString tokenAfter  = trigrams[stimulusID].at(3);
        bool found(false);
        for (int i = 1; i < tier_tokens->count() - 1; ++i) {
            QString target = tier_tokens->at(i)->text();
            if (target != tokenTarget) continue;
            int j = i - 1;
            while ((j > 0) && (tier_tokens->at(j)->isPauseSilent() || tier_tokens->at(j)->text() == "euh")) j--;
            QString before = tier_tokens->at(j)->text();
            if (before != tokenBefore) continue;
            int k = i + 1;
            while ((k < tier_tokens->count()) && (tier_tokens->at(k)->isPauseSilent() || tier_tokens->at(j)->text() == "euh")) k++;
            QString after = tier_tokens->at(k)->text();
            if (after != tokenAfter) continue;
            // found it
            tier_tokens->at(i)->setAttribute("target", "*");
            tier_units->merge(k, tier_tokens->count() - 2)->setText("S2");
            tier_units->at(i)->setText("MD");
            tier_units->merge(1, j)->setText("S1");
            found = true;
            break;
        }
        com->repository()->annotations()->saveTier(annotationID, speakerID, tier_tokens);
        com->repository()->annotations()->saveTier(annotationID, speakerID, tier_units);
        ret.append(com->ID()).append(found ? " OK\n" : " Trigram not found\n");
        delete tier_tokens;
        delete tier_units;
    }
    return ret;
}

QString ExperimentUtterances::rereadCorrectedTGs(QPointer<Praaline::Core::CorpusCommunication> com)
{
    QString ret;
    if (!com) return "Error";
    QPointer<Corpus> corpus = com->corpus();
    if (!corpus) return "Error";
    QString path = QDir::homePath() + "/Dropbox/MIS_Phradico/Experiences/03_Production-prosodie-relations-de-discours/Production/CORPUS_REPONSES/SX1/";
    QStringList filenameTGs;
    filenameTGs << "S0";

    foreach (QString filenameTG, filenameTGs) {
        AnnotationTierGroup *txg = new AnnotationTierGroup();
        if (!PraatTextGrid::load(path + filenameTG + ".TextGrid", txg)) {
            ret.append("Error ").append(filenameTG).append("\n");
            continue;
        }
        IntervalTier *tier_ID = txg->getIntervalTierByName("ID");
        if (!tier_ID) { ret.append("Error ").append(filenameTG).append("\n"); continue; }
        QStringList tierNames; tierNames << "phone" << "syll" << "tok_min";
        foreach (Interval *ID, tier_ID->intervals()) {
            QString speakerID = filenameTG.left(3);
            foreach (QString tierName, tierNames) {
                IntervalTier *tier_tg = txg->getIntervalTierByName(tierName);
                if (!tier_tg) { ret.append("Error opening textgrid tier ").append(filenameTG).append("\n"); continue; }
                IntervalTier *tier = qobject_cast<IntervalTier *>(corpus->repository()->annotations()->getTier(ID->text(), speakerID, tierName));
                if (!tier) { ret.append("Error opening database tier ").append(tierName).append(filenameTG).append("\n"); continue; }
                QList<Interval *> intervals;
                RealTime tFrom = ID->tMin(); if (tFrom < tier_tg->tMin()) tFrom = tier_tg->tMin();
                RealTime tTo = ID->tMax(); if (tTo > tier_tg->tMax()) tTo = tier_tg->tMax();
                foreach (Interval *intv, tier_tg->getIntervalsContainedIn(tFrom, tTo)) {
                    intervals << new Interval(intv->tMin() - ID->tMin(), intv->tMax() - ID->tMin(), intv->text());
                }
                tier->replaceAllIntervals(intervals);
                tier->fillEmptyWith("", "_");
                tier->mergeIdenticalAnnotations("_");
                com->repository()->annotations()->saveTier(ID->text(), speakerID, tier);
                delete tier;
            }
            ret.append(ID->text()).append(" OK\n");
        }
        delete txg;
    }
    return ret;
}

bool containsVowel(QString syll) {
    QStringList vowels;
    vowels << "i" << "e" << "E" << "a" << "A" << "o" << "O" << "y" << "2" << "9" << "@";
    // nasals are covered "e~" << "a~" << "o~" << "9~"
    for (int i = 0; i < syll.length(); ++i) {
        QString p = syll.mid(i, 1);
        if (vowels.contains(p)) return true;
    }
    return false;
}

QString ExperimentUtterances::resyllabifyMDs(QPointer<Praaline::Core::CorpusCommunication> com)
{
    QString ret;
    if (!com) return "Error";
    QPointer<CorpusRecording> rec = com->recordings().first();
    if (!rec) return "Error";
    QString annotationID = rec->ID();
    QString speakerID = com->property("SubjectID").toString();
    IntervalTier *tier_phone = qobject_cast<IntervalTier *>
            (com->repository()->annotations()->getTier(annotationID, speakerID, "phone"));
    IntervalTier *tier_syll = qobject_cast<IntervalTier *>
            (com->repository()->annotations()->getTier(annotationID, speakerID, "syll"));
    IntervalTier *tier_tokens = qobject_cast<IntervalTier *>
            (com->repository()->annotations()->getTier(annotationID, speakerID, "tok_min"));

    tier_syll->fixBoundariesBasedOnTier(tier_phone);

    Interval *target(0);
    foreach (Interval *intv, tier_tokens->intervals()) {
        if (intv->attribute("target").toString() == "*") target = intv;
    }
    if (!target) { ret.append(com->ID()).append(" target not found"); return ret; }
    QPair<int, int> indicesPhone = tier_phone->getIntervalIndexesContainedIn(target);
    QPair<int, int> indicesSyll = tier_syll->getIntervalIndexesOverlappingWith(target);
    ret.append(com->ID()).append("\t").append(target->text()).append("\t");
    for (int ip = indicesPhone.first; (ip > 0) && (ip <= indicesPhone.second); ++ip)
        ret.append(tier_phone->at(ip)->text());
    ret.append("\t");
    for (int is = indicesSyll.first; (is > 0) && (is <= indicesSyll.second); ++is)
        ret.append(tier_syll->at(is)->text()).append(".");
    if (ret.endsWith(".")) ret.chop(1);
    if ((indicesPhone.first < 0) || (indicesPhone.second < 0)) { ret.append(" phones not found"); return ret; }
    ret.append("\t");

    RealTime tMinFirstPhoneme = tier_phone->at(indicesPhone.first)->tMin();
    RealTime tMaxLastPhoneme = tier_phone->at(indicesPhone.second)->tMax();

    bool splitFirst(false), splitLast(false);
    int indexSyllAtFirstPhoneme = tier_syll->intervalIndexAtTime(tMinFirstPhoneme);
    if (tier_syll->at(indexSyllAtFirstPhoneme)->tMin() != tMinFirstPhoneme) {
        ret.append(" split first ");
        splitFirst = true;
        tier_syll->split(indexSyllAtFirstPhoneme, tMinFirstPhoneme);
        QString before;
        foreach (Interval *intv, tier_phone->getIntervalsContainedIn(tier_syll->at(indexSyllAtFirstPhoneme)))
            before = before.append(intv->text());       
        tier_syll->at(indexSyllAtFirstPhoneme)->setText(before);
        QString after;
        foreach (Interval *intv, tier_phone->getIntervalsContainedIn(tier_syll->at(indexSyllAtFirstPhoneme + 1)))
            after = after.append(intv->text());
        tier_syll->at(indexSyllAtFirstPhoneme + 1)->setText(after);
        // Check
        if (!containsVowel(tier_syll->at(indexSyllAtFirstPhoneme)->text())) {
            tier_syll->merge(indexSyllAtFirstPhoneme - 1, indexSyllAtFirstPhoneme);
        }
        // Return
        indexSyllAtFirstPhoneme = tier_syll->intervalIndexAtTime(tMinFirstPhoneme);
        for (int i = -1; i <= 1; ++i)
            ret.append(tier_syll->at(indexSyllAtFirstPhoneme + i)->text()).append("|");
        if (ret.endsWith("|")) ret.chop(1);
    }
    int indexSyllAtLastPhoneme = tier_syll->intervalIndexAtTime(tMaxLastPhoneme, true);
    if (tier_syll->at(indexSyllAtLastPhoneme)->tMax() != tMaxLastPhoneme) {
        ret.append(" split last ");
        splitLast = true;
        tier_syll->split(indexSyllAtLastPhoneme, tMaxLastPhoneme);
        QString before;
        foreach (Interval *intv, tier_phone->getIntervalsContainedIn(tier_syll->at(indexSyllAtLastPhoneme)))
            before = before.append(intv->text());
        tier_syll->at(indexSyllAtLastPhoneme)->setText(before);
        QString after;
        foreach (Interval *intv, tier_phone->getIntervalsContainedIn(tier_syll->at(indexSyllAtLastPhoneme + 1)))
            after = after.append(intv->text());
        tier_syll->at(indexSyllAtLastPhoneme + 1)->setText(after);
        // Check
        if (!containsVowel(tier_syll->at(indexSyllAtLastPhoneme)->text())) {
            tier_syll->merge(indexSyllAtLastPhoneme - 1, indexSyllAtLastPhoneme);
        }
        // Return
        indexSyllAtLastPhoneme = tier_syll->intervalIndexAtTime(tMaxLastPhoneme, true);
        for (int i = -1; i <= 1; ++i)
            ret.append(tier_syll->at(indexSyllAtLastPhoneme + i)->text()).append("|");
        if (ret.endsWith("|")) ret.chop(1);
    }

    if (!splitFirst && !splitLast) ret = "";
    com->repository()->annotations()->saveTier(annotationID, speakerID, tier_syll);
    delete tier_phone;
    delete tier_syll;
    delete tier_tokens;
    return ret;
}