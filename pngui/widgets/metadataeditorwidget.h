#ifndef METADATAEDITORWIDGET_H
#define METADATAEDITORWIDGET_H

#include <QWidget>
#include <QString>
#include <QPointer>
#include <QtProperty>
#include <QPair>
#include <QMap>
#include "pncore/corpus/corpusobject.h"
#include "pncore/structure/metadatastructure.h"

class MetadataEditorWidget : public QWidget
{
    Q_OBJECT
public:
    class PropertyID {
    public:
        PropertyID() : type(CorpusObject::Type_Undefined), itemID(QString()), attributeID(QString()) {}
        PropertyID(CorpusObject::Type type, QString itemID, QString attributeID) :
            type(type), itemID(itemID), attributeID(attributeID) {}
        CorpusObject::Type type;
        QString itemID;
        QString attributeID;
    };

    MetadataEditorWidget(QWidget *parent = 0);
    ~MetadataEditorWidget();

    void rebind(QPointer<MetadataStructure> mstructure, QList<QPointer<CorpusObject> > &items,
                bool includeParticipationSpeaker = false, bool includeParticipationCommunication = false);
    void clear();

signals:

public slots:

private slots:
    void valueChanged(QtProperty *property, const QString &value);
    void valueChanged(QtProperty *property, bool value);
    void valueChanged(QtProperty *property, int value);
    void valueChanged(QtProperty *property, double value);
    void valueChanged(QtProperty *property, const QDate &value);

private:
    class QtStringPropertyManager *m_stringManager;
    class QtBoolPropertyManager *m_boolManager;
    class QtIntPropertyManager *m_intManager;
    class QtDoublePropertyManager *m_doubleManager;
    class QtEnumPropertyManager *m_enumManager;
    class QtDatePropertyManager *m_dateManager;
    class QtGroupPropertyManager *m_groupManager;

    class QtTreePropertyBrowser *m_propertyEditor;

    QMap<QPair<CorpusObject::Type, QString>, QPointer<CorpusObject> > m_items;

    QMap<QtProperty *, PropertyID> m_propertyToId;
    QMap<PropertyID, QtProperty *> m_idToProperty;

    void addProperty(QtProperty *property, CorpusObject::Type type, const QString &itemID, const QString &attributeID, QtProperty *group);
    QtProperty *addProperties(QPointer<MetadataStructure> mstructure, QPointer<CorpusObject> item, QtProperty *under = 0);

};

inline bool operator<(const MetadataEditorWidget::PropertyID &id1, const MetadataEditorWidget::PropertyID &id2)
{
    if (id1.type != id2.type) return id1.type < id2.type;
    else if (id1.itemID != id2.itemID) return id1.itemID < id2.itemID;
    return id1.attributeID < id2.attributeID;
}

#endif // METADATAEDITORWIDGET_H
