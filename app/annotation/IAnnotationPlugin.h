#ifndef IANNOTATIONPLUGIN_H
#define IANNOTATIONPLUGIN_H

#include <QString>
#include <QList>
#include <QHash>
#include <QPointer>
#include <IPlugin>
using namespace Qtilities::ExtensionSystem::Interfaces;
#include "pncore/corpus/corpus.h"
#include "pncore/corpus/corpuscommunication.h"

namespace Praaline {
    namespace Plugins {
        class IAnnotationPlugin : public IPlugin
        {
        public:
            class PluginParameter {
            public:
                QString ID;
                QString name;
                int propertyType;
                QVariant defaultValue;
                QStringList listOptions;
                QString description;

                PluginParameter(QString ID, QString name, int propertyType, QVariant defaultValue = QVariant(),
                                QStringList listOptions = QStringList(), QString description = QString()) :
                    ID(ID), name(name), propertyType(propertyType), defaultValue(defaultValue),
                    listOptions(listOptions), description(description)
                {}
            };

            IAnnotationPlugin() : IPlugin() {}
            virtual ~IAnnotationPlugin() {}

            virtual QList<PluginParameter> pluginParameters() const = 0;
            virtual void setParameters(QHash<QString, QVariant> parameters) = 0;
            virtual void process(Corpus *corpus, QList<QPointer<CorpusCommunication> > communications) = 0;
        signals:
            virtual void printMessage(QString message) = 0 ;
            virtual void madeProgress(int progress) = 0;

        public slots:
        };
    }
}

Q_DECLARE_INTERFACE(Praaline::Plugins::IAnnotationPlugin, "org.Praaline.Plugins.IAnnotationPlugin/1.0")

#endif // IANNOTATIONPLUGIN_H