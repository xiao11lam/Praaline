#ifndef PHONETISER_H
#define PHONETISER_H

#include <QObject>
#include <QList>

namespace Praaline {

namespace Core {
class Interval;
}

namespace ASR {

class Phonetiser : public QObject
{
    Q_OBJECT
public:
    explicit Phonetiser(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~Phonetiser() {}

    virtual QString phonetiseWord(const QString &word) = 0;
    virtual QList<Core::Interval *> phonetiseUtterance(Core::Interval *utterance) = 0;

signals:

public slots:
};

} // namespace ASR
} // namespace Praaline

#endif // PHONETISER_H
