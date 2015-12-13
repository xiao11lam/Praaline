#ifndef DISMOSERIALISERSQL_H
#define DISMOSERIALISERSQL_H

#include <QObject>
#include <QSqlDatabase>
#include "dismo/tokenlist.h"

namespace DisMoAnnotator {

class DisMoSerialiserSql : public QObject
{
    Q_OBJECT
public:
    explicit DisMoSerialiserSql(QObject *parent = 0);
    static void serializeToDatabase(TokenList &TL, QSqlDatabase &db, const QString &annotationID, const QString &speakerID);

signals:

public slots:

};

}

#endif // DISMOSERIALISERSQL_H
