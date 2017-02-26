#ifndef ANALYSERPITCH_H
#define ANALYSERPITCH_H

#include <QObject>
#include <QPointer>
#include <QString>
#include <QList>
#include <QStandardItemModel>

namespace Praaline {
namespace Core {
class Corpus;
class CorpusCommunication;
class Interval;
class IntervalTier;
class StatisticalMeasureDefinition;
}
}

struct AnalyserMacroprosodyData;

class AnalyserMacroprosody : public QObject
{
    Q_OBJECT
public:
    explicit AnalyserMacroprosody(QObject *parent = 0);
    virtual ~AnalyserMacroprosody();

    static QList<QString> groupingLevels();
    static QList<QString> measureIDs(const QString &groupingLevel);
    static Praaline::Core::StatisticalMeasureDefinition measureDefinition(const QString &groupingLevel, const QString &measureID);

    double measure(const QString &groupingLevel, const QString &key, const QString &measureID) const;

    QPointer<QStandardItemModel> model();

    QString calculate(QPointer<Praaline::Core::Corpus> corpus, QPointer<Praaline::Core::CorpusCommunication> com);
    QString calculate(QPointer<Praaline::Core::Corpus> corpus,
                      const QString &communicationID, const QString &annotationID, const QString &speakerIDfilter = QString(),
                      const QList<Praaline::Core::Interval *> &units = QList<Praaline::Core::Interval *>());


signals:

public slots:

private:
    AnalyserMacroprosodyData *d;
};

#endif // ANALYSERPITCH_H