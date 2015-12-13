#ifndef XMLANNOTATIONDATASTORE_H
#define XMLANNOTATIONDATASTORE_H

#include <QObject>
#include "serialisers/abstractannotationdatastore.h"

class XMLAnnotationDatastore : public AbstractAnnotationDatastore
{
    Q_OBJECT
public:
    explicit XMLAnnotationDatastore(QPointer<AnnotationStructure> structure, QObject *parent = 0);
    ~XMLAnnotationDatastore();

    bool createDatastore(const DatastoreInfo &info);
    bool openDatastore(const DatastoreInfo &info);
    bool closeDatastore();

    bool loadAnnotationStructure();
    bool saveAnnotationStructure();
    bool createAnnotationLevel(QPointer<AnnotationStructureLevel> newLevel);
    bool renameAnnotationLevel(QString levelID, QString newLevelID);
    bool deleteAnnotationLevel(QString levelID);
    bool createAnnotationAttribute(QString levelID, QPointer<AnnotationStructureAttribute> newAttribute);
    bool renameAnnotationAttribute(QString levelID, QString attributeID, QString newAttributeID);
    bool deleteAnnotationAttribute(QString levelID, QString attributeID);
    bool retypeAnnotationAttribute(QString levelID, QString attributeID, QString newDatatype, int newDatalength);

    bool saveTiersAllSpeakers(const QString &annotationID, QMap<QString, QPointer<AnnotationTierGroup> > &tiersAllSpeakers);
    bool saveTiers(const QString &annotationID, const QString &speakerID, AnnotationTierGroup *tiers);
    bool saveTier(const QString &annotationID, const QString &speakerID, AnnotationTier *tier);

    AnnotationTierGroup *getTiers(const QString &annotationID, const QString &speakerID,
                                  const QStringList &levelIDs = QStringList());
    QMap<QString, QPointer<AnnotationTierGroup> > getTiersAllSpeakers(const QString &annotationID,
                                                                      const QStringList &levelIDs = QStringList());
    AnnotationTier *getTier(const QString &annotationID, const QString &speakerID,
                            const QString &levelID, const QStringList &attributeIDs = QStringList());
    QList<Interval *> getIntervals(const QString &annotationID, const QString &speakerID,
                                   const QString &levelID, int intervalNoMin, int intervalNoMax,
                                   const QStringList &attributeIDs = QStringList());
    QList<Interval *> getIntervals(const QString &annotationID, const QString &speakerID,
                                   const QString &levelID, RealTime tMin, RealTime tMax,
                                   const QStringList &attributeIDs = QStringList());

    QList<QString> getSpeakersInLevel(const QString &annotationID, const QString &levelID);
    QList<QString> getSpeakersActiveInLevel(const QString &annotationID, const QString &levelID);
    QList<QString> getSpeakersInAnnotation(const QString &annotationID);
    QList<QString> getSpeakersActiveInAnnotation(const QString &annotationID);
    IntervalTier *getSpeakerTimeline(const QString &annotationID, const QString &levelID);

    bool deleteAllForAnnotationID(QString annotationID);

    QList<QueryOccurrencePointer *> runQuery(QueryDefinition *qdef);
    QueryOccurrence *getOccurrence(QueryOccurrencePointer *pointer, QueryDefinition *qdef);
    bool updateAnnotationsFromQueryOccurrences(const QList<QueryOccurrence *> &occurrences);


private:
    QString m_folder;
    QPointer<AnnotationStructure> m_structure;
};

#endif // XMLANNOTATIONDATASTORE_H