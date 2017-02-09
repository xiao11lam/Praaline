#include <QDebug>
#include <QApplication>
#include <QProgressBar>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QStandardItemModel>

#include "pncore/corpus/Corpus.h"
#include "pncore/corpus/CorpusCommunication.h"
#include "pncore/corpus/CorpusSpeaker.h"
#include "pncore/structure/MetadataStructure.h"
#include "pncore/structure/AnnotationStructure.h"
#include "pncore/datastore/CorpusRepository.h"
#include "pncore/datastore/MetadataDatastore.h"
#include "pncore/datastore/AnnotationDatastore.h"
#include "pncore/annotation/AnnotationTierGroup.h"
#include "pncore/annotation/IntervalTier.h"
using namespace Praaline::Core;

#include "pngui/widgets/GridViewWidget.h"
#include "AnalyserMacroprosodyWidget.h"
#include "ui_AnalyserMacroprosodyWidget.h"

#include "AnalyserMacroprosody.h"

namespace Praaline {
namespace Plugins {
namespace StatisticsPluginProsody {

struct AnalyserMacroprosodyWidgetData {
    AnalyserMacroprosodyWidgetData() :
        repository(0), gridviewResults(0), modelResults(0)
    {}

    CorpusRepository *repository;
    GridViewWidget *gridviewResults;
    QStandardItemModel *modelResults;
};

AnalyserMacroprosodyWidget::AnalyserMacroprosodyWidget(CorpusRepository *repository, QWidget *parent) :
    QWidget(parent), ui(new Ui::AnalyserMacroprosodyWidget), d(new AnalyserMacroprosodyWidgetData)
{
    ui->setupUi(this);
    if (!repository) return;
    d->repository = repository;
    // Corpora combobox
    ui->comboBoxCorpus->addItems(repository->listCorporaIDs());
    // Levels from annotation structure
    ui->comboBoxMUTier->addItems(repository->annotationStructure()->levelIDs());
    // Command Analyse
    connect(ui->commandAnalyse, SIGNAL(clicked(bool)), this, SLOT(analyse()));
    // Command Open File for MU list
    connect(ui->commandOpenFileMUList, SIGNAL(clicked(bool)), this, SLOT(openFileMUList()));
    // Results grid view
    d->gridviewResults = new GridViewWidget(this);
    d->gridviewResults->tableView()->verticalHeader()->setDefaultSectionSize(20);
    ui->gridLayoutResults->addWidget(d->gridviewResults);
    // Default options
    ui->optionMULIstFromTier->setChecked(true);
}

AnalyserMacroprosodyWidget::~AnalyserMacroprosodyWidget()
{
    delete ui;
    delete d;
}

void AnalyserMacroprosodyWidget::openFileMUList()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QString filename = QFileDialog::getOpenFileName(this, tr("Select File for Macro-Unit List"), "",
                                                    tr("Text File (*.txt);;All Files (*)"),
                                                    &selectedFilter, options);
    if (filename.isEmpty()) return;
    ui->editFilenameMUList->setText(filename);
    ui->optionMUListFromFile->setChecked(true);
}

void AnalyserMacroprosodyWidget::analyse()
{
    if (!d->repository) return;
    QString corpusID = ui->comboBoxCorpus->currentText();
    QPointer<Corpus> corpus = d->repository->metadata()->getCorpus(corpusID);
    if (!corpus) return;

    QScopedPointer<AnalyserMacroprosody> analyser(new AnalyserMacroprosody);

    QStandardItemModel *model = new QStandardItemModel(this);
    if (ui->optionMULIstFromTier->isChecked()) {
        foreach (QPointer<CorpusCommunication> com, corpus->communications()) {
            if (!com) continue;
            foreach (QString annotationID, com->annotationIDs()) {
                QString tierNameMacroUnits = ui->comboBoxMUTier->currentText();
                QMap<QString, QPointer<AnnotationTierGroup> > tiersAll =
                        d->repository->annotations()->getTiersAllSpeakers(annotationID, QStringList() << tierNameMacroUnits);
                foreach (QString speakerID, tiersAll.keys()) {
                    QPointer<AnnotationTierGroup> tiers = tiersAll.value(speakerID);
                    if (!tiers) continue;
                    IntervalTier *tier_macroUnit = tiers->getIntervalTierByName(tierNameMacroUnits);
                    QList<Interval *> macroUnitIntervals;
                    foreach (Interval *intv, tier_macroUnit->intervals()) {
                        if (!intv->isPauseSilent()) macroUnitIntervals << intv;
                    }
                    // Run analyser for each macro-unit (excluding pauses) and take each row of results into the model
                    analyser->calculate(corpus, com->ID(), annotationID, speakerID, macroUnitIntervals);
                    for (int i = 0; i < analyser->model()->rowCount(); ++i) {
                        model->appendRow(analyser->model()->takeRow(i));
                    }
                }
                qDeleteAll(tiersAll);
            }
        }
    } else {
        QFile file(ui->editFilenameMUList->text());
        if (!file.open( QIODevice::ReadOnly | QIODevice::Text )) return;
        QTextStream stream(&file);
        QList<Interval *> intervals;
        QString currentCommunicationID, currentAnnotationID, currentSpeakerID;
        QStringList fileLines;
        do {
            QString line = stream.readLine().trimmed();
            fileLines << line;
        } while (!stream.atEnd());
        file.close();
        ui->progressBar->setMaximum(fileLines.count());
        int lineNo(0);
        foreach (QString line, fileLines) {
            // Format to use:
            // CommunicationID | AnnotationID | SpeakerID | Occurrence | Part | tMin | tMax
            // =============================================================================
            QString communicationID = line.section("\t", 0, 0);
            QString annotationID = line.section("\t", 1, 1);
            QString speakerID = line.section("\t", 2, 2);
            QString label = line.section("\t", 3, 4);
            RealTime tMin = RealTime::fromSeconds(line.section("\t", 5, 5).toDouble());
            RealTime tMax = RealTime::fromSeconds(line.section("\t", 6, 6).toDouble());
            if ((communicationID == currentCommunicationID) && (annotationID == currentAnnotationID) && (speakerID == currentSpeakerID)) {
                intervals << new Interval(tMin, tMax, label);
                // qDebug() << currentCommunicationID << currentAnnotationID << currentSpeakerID << label;
            }
            else {
                if (!intervals.isEmpty()) {
                    analyser->calculate(corpus, currentCommunicationID, currentAnnotationID, currentSpeakerID, intervals);
                    while (analyser->model()->rowCount() > 0) {
                        model->appendRow(analyser->model()->takeRow(0));
                    }
                    qDeleteAll(intervals);
                    intervals.clear();
                }
                currentCommunicationID = communicationID;
                currentAnnotationID = annotationID;
                currentSpeakerID = speakerID;
                intervals << new Interval(tMin, tMax, label);
                // qDebug() << currentCommunicationID << currentAnnotationID << currentSpeakerID << label;
            }
            lineNo++;
            ui->progressBar->setValue(lineNo);
            QApplication::processEvents();
        }
        // Any intervals left?
        if (!intervals.isEmpty()) {
            analyser->calculate(corpus, currentCommunicationID, currentAnnotationID, currentSpeakerID, intervals);
            while (analyser->model()->rowCount() > 0) {
                model->appendRow(analyser->model()->takeRow(0));
            }
            qDeleteAll(intervals);
            intervals.clear();
        }
    }
    // Update table headers
    if (analyser && analyser->model()) {
        for (int i = 0; i < analyser->model()->columnCount(); ++i)
            model->setHorizontalHeaderItem(i, new QStandardItem(analyser->model()->horizontalHeaderItem(i)->text()));
    }
    // Update table
    d->gridviewResults->tableView()->setModel(model);
    if (d->modelResults) { d->modelResults->clear(); delete d->modelResults; }
    d->modelResults = model;
}

} // namespace StatisticsPluginProsody
} // namespace Plugins
} // namespace Praaline
