#include <QDebug>
#include <QPointer>
#include <QtPlugin>
#include <QIcon>
#include <QApplication>
#include <ExtensionSystemConstants>

#include "pluginsyntax.h"
#include "pncore/corpus/corpus.h"
#include "pncore/interfaces/praat/praattextgrid.h"

using namespace Qtilities::ExtensionSystem;
using namespace Praaline::Plugins;

struct Praaline::Plugins::Syntax::PluginSyntaxPrivateData {
    PluginSyntaxPrivateData() {}
};

Praaline::Plugins::Syntax::PluginSyntax::PluginSyntax(QObject* parent) : QObject(parent)
{
    d = new PluginSyntaxPrivateData;
    setObjectName(pluginName());
}

Praaline::Plugins::Syntax::PluginSyntax::~PluginSyntax()
{
    delete d;
}

bool Praaline::Plugins::Syntax::PluginSyntax::initialize(const QStringList &arguments, QStringList *error_strings) {
    Q_UNUSED(arguments)
    Q_UNUSED(error_strings)

    return true;
}

bool Praaline::Plugins::Syntax::PluginSyntax::initializeDependencies(QStringList *error_strings) {
    Q_UNUSED(error_strings)

    return true;
}

void Praaline::Plugins::Syntax::PluginSyntax::finalize() {

}

QString Praaline::Plugins::Syntax::PluginSyntax::pluginName() const {
    return "Syntax";
}

QtilitiesCategory Praaline::Plugins::Syntax::PluginSyntax::pluginCategory() const {
    return QtilitiesCategory(QApplication::applicationName());
}

Qtilities::Core::VersionInformation Praaline::Plugins::Syntax::PluginSyntax::pluginVersionInformation() const {
    VersionInformation version_info(1, 0, 0);
    return version_info;
}

QString Praaline::Plugins::Syntax::PluginSyntax::pluginPublisher() const {
    return "George Christodoulides";
}

QString Praaline::Plugins::Syntax::PluginSyntax::pluginPublisherWebsite() const {
    return "http://www.corpusannotation.org";
}

QString Praaline::Plugins::Syntax::PluginSyntax::pluginPublisherContact() const {
    return "info@corpusannotation.org";
}

QString Praaline::Plugins::Syntax::PluginSyntax::pluginDescription() const {
    return tr("Syntax is a part-of-speech tagger, multi-word unit and disfluency detector for spoken language transcriptions.");
}

QString Praaline::Plugins::Syntax::PluginSyntax::pluginCopyright() const {
    return QString(tr("Copyright") + " 2012-2014, George Christodoulides");
}

QString Praaline::Plugins::Syntax::PluginSyntax::pluginLicense() const {
    return tr("GPL v.3");
}

QList<IAnnotationPlugin::PluginParameter> Praaline::Plugins::Syntax::PluginSyntax::pluginParameters() const
{
    QList<IAnnotationPlugin::PluginParameter> parameters;
    return parameters;
}

void Praaline::Plugins::Syntax::PluginSyntax::setParameters(QHash<QString, QVariant> parameters)
{
}

void Praaline::Plugins::Syntax::PluginSyntax::process(Corpus *corpus, QList<QPointer<CorpusCommunication> > communications)
{
    QMap<QString, QPointer<AnnotationTierGroup> > tiersAll;
    foreach (QPointer<CorpusCommunication> com, communications) {
        if (!com) continue;
        if (!com->hasRecordings()) continue;
        QPointer<CorpusRecording> rec = com->recordings().first();
        foreach (QPointer<CorpusAnnotation> annot, com->annotations()) {
            if (!annot) continue;
            QString annotationID = annot->ID();
            tiersAll = corpus->datastoreAnnotations()->getTiersAllSpeakers(annotationID);
            foreach (QString speakerID, tiersAll.keys()) {
                QPointer<AnnotationTierGroup> tiers = tiersAll.value(speakerID);
                if (!tiers) continue;

                // corpus->datastoreAnnotations()->saveTiers(annot->ID(), speakerID, tiers);
            }
            qDeleteAll(tiersAll);
        }
    }
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    using namespace Praaline::Plugins::Syntax;
    Q_EXPORT_PLUGIN2(PluginSyntax, PluginSyntax)
#endif