#ifndef SPHINXCONTINUOUSRECOGNISER_H
#define SPHINXCONTINUOUSRECOGNISER_H

#include <QObject>

class SphinxContinuousRecogniser : public QObject
{
    Q_OBJECT
public:
    explicit SphinxContinuousRecogniser(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SPHINXCONTINUOUSRECOGNISER_H