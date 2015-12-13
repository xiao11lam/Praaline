#ifndef QUERYOCCURRENCE_H
#define QUERYOCCURRENCE_H

#include "pncore_global.h"
#include <QObject>
#include <QString>
#include <QMap>
#include "pncore/annotation/interval.h"

struct PRAALINE_CORE_SHARED_EXPORT QueryOccurrencePointer
{
    QString corpusID;
    QString communicationID;
    QString annotationID;
    QString speakerID;
    QString annotationLevelID;
    int intervalNoMin;
    int intervalNoMax;
    long long tMin_nsec;
    long long tMax_nsec;
};

class PRAALINE_CORE_SHARED_EXPORT QueryOccurrence : public QObject
{
    Q_OBJECT
public:
    class ResultInterval {
    public:
        enum Type { LeftContext, Target, RightContext };

        ResultInterval() : type(ResultInterval::Target), intervalNo(0), interval(0)
        {}
        ResultInterval(ResultInterval::Type type, const QString &speakerID, int intervalNo, Interval *interval) :
            type(type), speakerID(speakerID), intervalNo(intervalNo), interval(interval)
        {}

        Type type;
        QString speakerID;
        int intervalNo;
        Interval *interval;
    };

    explicit QueryOccurrence(const QString &corpusID, const QString &communicationID, const QString &annotationID,
                             const QMap<QString, QList<ResultInterval> > &resultIntervals,
                             QObject *parent = 0);
    ~QueryOccurrence();

    QString corpusID() const { return m_corpusID; }
    QString communicationID() const { return m_communicationID; }
    QString annotationID() const { return m_annotationID; }
    RealTime tMin() const;
    RealTime tMax() const;
    QStringList annotationLevelIDs() const;
    QStringList speakerIDs() const;

    // lazy load mechanism
    bool loaded() const { return m_loaded; }
    QList<ResultInterval> resultIntervals(const QString &annotationLevelID);
    int resultIntervalsCount(const QString &annotationLevelID) const;
    QList<ResultInterval> leftContext(const QString &annotationLevelID);
    QList<ResultInterval> target(const QString &annotationLevelID);
    QList<ResultInterval> rightContext(const QString &annotationLevelID);

signals:

public slots:

protected:
    QString m_corpusID;
    QString m_communicationID;
    QString m_annotationID;
    // Annotation level : a sequence of result intervals
    bool m_loaded;
    QMap<QString, QList<ResultInterval> > m_resultIntervals;
};

#endif // QUERYOCCURRENCE_H