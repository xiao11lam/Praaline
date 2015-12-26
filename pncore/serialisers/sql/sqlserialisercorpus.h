#ifndef SQLSERIALISERCORPUS_H
#define SQLSERIALISERCORPUS_H

#include <QSqlDatabase>
#include "sqlserialiserbase.h"
#include "structure/metadatastructure.h"
#include "corpus/corpus.h"
#include "corpus/corpuscommunication.h"
#include "corpus/corpusspeaker.h"
#include "corpus/corpusrecording.h"
#include "corpus/corpusannotation.h"
#include "corpus/corpusparticipation.h"

class SQLSerialiserCorpus : public SQLSerialiserBase
{
public:
    static bool saveCorpus(Corpus *corpus, QSqlDatabase &db);
    static bool loadCorpus(Corpus *corpus, QSqlDatabase &db);

    static bool saveCommunication(CorpusCommunication *com, MetadataStructure *structure, QSqlDatabase &db);
    static bool saveSpeaker(CorpusSpeaker *spk, MetadataStructure *structure, QSqlDatabase &db);
    static bool saveParticipation(CorpusParticipation *participation, MetadataStructure *structure, QSqlDatabase &db);

    static QList<CorpusCommunication *> getCommunications(QString corpusID, MetadataStructure *structure, QSqlDatabase &db);
    static QList<CorpusSpeaker *> getSpeakers(QString corpusID, MetadataStructure *structure, QSqlDatabase &db);
    static QList<CorpusRecording *> getRecordings(QString communicationID, MetadataStructure *structure, QSqlDatabase &db);
    static QList<CorpusAnnotation *> getAnnotations(QString communicationID, MetadataStructure *structure, QSqlDatabase &db);
    static QMultiMap<QString, CorpusRecording *> getRecordingsAll(QString corpusID, MetadataStructure *structure, QSqlDatabase &db);
    static QMultiMap<QString, CorpusAnnotation *> getAnnotationsAll(QString corpusID, MetadataStructure *structure, QSqlDatabase &db);

private:
    SQLSerialiserCorpus();

    static QList<CorpusCommunication *> execGetCommunications(QString corpusID, MetadataStructure *structure, QSqlDatabase &db);
    static QList<CorpusSpeaker *> execGetSpeakers(QString corpusID, MetadataStructure *structure, QSqlDatabase &db);
    static QList<CorpusRecording *> execGetRecordings(QString communicationID, MetadataStructure *structure, QSqlDatabase &db);
    static QList<CorpusAnnotation *> execGetAnnotations(QString communicationID, MetadataStructure *structure, QSqlDatabase &db);
    static QMultiMap<QString, CorpusRecording *> execGetRecordingsAll(QString corpusID, MetadataStructure *structure, QSqlDatabase &db);
    static QMultiMap<QString, CorpusAnnotation *> execGetAnnotationsAll(QString corpusID, MetadataStructure *structure, QSqlDatabase &db);
    static void execGetParticipations(Corpus *corpus, MetadataStructure *structure, QSqlDatabase &db);

    static bool execSaveCommunication(CorpusCommunication *com, MetadataStructure *structure, QSqlDatabase &db);
    static bool execSaveSpeaker(CorpusSpeaker *spk, MetadataStructure *structure, QSqlDatabase &db);
    static bool execSaveRecording(QString communicationID, CorpusRecording *rec, MetadataStructure *structure, QSqlDatabase &db);
    static bool execSaveAnnotation(QString communicationID, CorpusAnnotation *annot, MetadataStructure *structure, QSqlDatabase &db);
    static bool execSaveParticipation(CorpusParticipation *participation, MetadataStructure *structure, QSqlDatabase &db);
    static bool execCleanUpCommunication(CorpusCommunication *com, QSqlDatabase &db);
};

#endif // SQLSERIALISERCORPUS_H
