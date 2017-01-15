#include <QObject>
#include <QPointer>
#include <QString>
#include <QList>
#include <QMetaEnum>
#include <QRegularExpression>

#include "structure/MetadataStructure.h"
#include "structure/AnnotationStructure.h"
#include "CorpusRepositoryDefinition.h"
#include "DatastoreFactory.h"
#include "MetadataDatastore.h"
#include "AnnotationDatastore.h"
#include "FileDatastore.h"
#include "CorpusRepository.h"

namespace Praaline {
namespace Core {

struct CorpusRepositoryData {
    QString lastError;
    QPointer<MetadataStructure> metadataStructure;
    QPointer<AnnotationStructure> annotationStructure;
    QPointer<MetadataDatastore> datastoreMetadata;
    QPointer<AnnotationDatastore> datastoreAnnotations;
    QPointer<FileDatastore> datastoreFiles;
};

CorpusRepository::CorpusRepository(const CorpusRepositoryDefinition &definition, QObject *parent) :
    QObject(parent), d(new CorpusRepositoryData)
{
    d->annotationStructure = new AnnotationStructure(this);
    d->metadataStructure = new MetadataStructure(this);
    d->datastoreAnnotations = DatastoreFactory::getAnnotationDatastore(definition.infoDatastoreAnnotations, d->annotationStructure, this);
    d->datastoreMetadata = DatastoreFactory::getMetadataDatastore(definition.infoDatastoreMetadata, d->metadataStructure, this);
    d->datastoreFiles = new FileDatastore(this);
}

CorpusRepository::~CorpusRepository()
{
    if (d->datastoreMetadata)    delete d->datastoreMetadata;
    if (d->datastoreAnnotations) delete d->datastoreAnnotations;
    if (d->datastoreFiles)       delete d->datastoreFiles;
    if (d->metadataStructure)    delete d->metadataStructure;
    if (d->annotationStructure)  delete d->annotationStructure;
    delete d;
}

// static
CorpusRepository *CorpusRepository::create(const CorpusRepositoryDefinition &definition, QString &errorMessages, QObject *parent)
{
    errorMessages.clear();
    CorpusRepository *repository = new CorpusRepository(definition, parent);
    repository->metadataStructure()->addSection(CorpusObject::Type_Corpus, new MetadataStructureSection("corpus", "Corpus", "(Default section)"));
    repository->metadataStructure()->addSection(CorpusObject::Type_Communication, new MetadataStructureSection("communication", "Communication", "(Default section)"));
    repository->metadataStructure()->addSection(CorpusObject::Type_Recording, new MetadataStructureSection("recording", "Recording", "(Default section)"));
    repository->metadataStructure()->addSection(CorpusObject::Type_Annotation, new MetadataStructureSection("annotation", "Annotation", "(Default section)"));
    repository->metadataStructure()->addSection(CorpusObject::Type_Speaker, new MetadataStructureSection("speaker", "Speaker", "(Default section)"));
    repository->metadataStructure()->addSection(CorpusObject::Type_Participation, new MetadataStructureSection("participation", "Participation", "(Default section)"));
    if (repository->annotations()) {
        if (!repository->annotations()->createDatastore(definition.infoDatastoreAnnotations)) {
            errorMessages.append("Error creating annotation datastore: ").append(repository->lastError()).append("\n");
            repository->clearLastError();
        }
    }
    if (repository->metadata()) {
        if (!repository->metadata()->createDatastore(definition.infoDatastoreMetadata)) {
            errorMessages.append("Error creating metadata datastore: ").append(repository->lastError()).append("\n");
            repository->clearLastError();
        }
    }
    if (repository->files()) {
        repository->files()->setBasePath(definition.basePathMedia);
    }
    return repository;
}

// static
CorpusRepository *CorpusRepository::open(const CorpusRepositoryDefinition &definition, QString &errorMessages, QObject *parent)
{
    errorMessages.clear();
    QPointer<CorpusRepository> repository = new CorpusRepository(definition, parent);
    if (repository->annotations()) {
        if (!repository->annotations()->openDatastore(definition.infoDatastoreAnnotations)) {
            errorMessages.append("Error opening annotation datastore: ").append(repository->lastError()).append("\n");
            repository->clearLastError();
            return Q_NULLPTR;
        }
        if (!repository->annotations()->loadAnnotationStructure()) {
            errorMessages.append("Error reading annotation structure: ").append(repository->lastError()).append("\n");
            repository->clearLastError();
            return Q_NULLPTR;
        }
    }
    if (repository->metadata()) {
        if (!repository->metadata()->openDatastore(definition.infoDatastoreMetadata)) {
            errorMessages.append("Error opening metadata datastore: ").append(repository->lastError()).append("\n");
            repository->clearLastError();
            return Q_NULLPTR;
        }
        if (!repository->metadata()->loadMetadataStructure()) {
            errorMessages.append("Error reading metadata structure: ").append(repository->lastError()).append("\n");
            repository->clearLastError();
            return Q_NULLPTR;
        }
    }
    if (repository->files()) {
        repository->files()->setBasePath(definition.basePathMedia);
    }
    return repository;
}

void CorpusRepository::save()
{
    d->datastoreMetadata->saveMetadataStructure();
    d->datastoreAnnotations->saveAnnotationStructure();
}

void CorpusRepository::close()
{
    d->datastoreMetadata->closeDatastore();
    d->datastoreAnnotations->closeDatastore();
}

// ==============================================================================================================================
// Datastores
// ==============================================================================================================================

QPointer<AnnotationDatastore> CorpusRepository::annotations() const
{
    return d->datastoreAnnotations;
}

QPointer<MetadataDatastore> CorpusRepository::metadata() const
{
    return d->datastoreMetadata;
}

QPointer<FileDatastore> CorpusRepository::files() const
{
    return d->datastoreFiles;
}

// ==============================================================================================================================
// Metadata and annotation structures
// ==============================================================================================================================

QPointer<MetadataStructure> CorpusRepository::metadataStructure() const
{
    return d->metadataStructure;
}

QPointer<AnnotationStructure> CorpusRepository::annotationStructure() const
{
    return d->annotationStructure;
}

void CorpusRepository::importMetadataStructure(MetadataStructure *otherStructure)
{
    if (!d->datastoreMetadata) return;
    if (!d->metadataStructure) return;

    const QMetaObject &mo = CorpusObject::staticMetaObject;
    int index = mo.indexOfEnumerator("Type");
    QMetaEnum metaEnum = mo.enumerator(index);
    for (int i = 0; i < metaEnum.keyCount(); ++i) {
        CorpusObject::Type type = static_cast<CorpusObject::Type>(metaEnum.value(i));
        MetadataStructureSection *mySection;
        foreach (MetadataStructureSection *otherSection, otherStructure->sections(type)) {
            mySection = d->metadataStructure->section(type, otherSection->ID());
            if (mySection) {
                // Copy over name and description, only if this structure does not already define them
                if (mySection->name().isEmpty())
                    mySection->setName(otherSection->name());
                if (mySection->description().isEmpty())
                    mySection->setDescription(otherSection->description());
                // Have section in common, check attributes.
                foreach (MetadataStructureAttribute *otherAttribute, otherSection->attributes()) {
                    MetadataStructureAttribute *myAttribute = mySection->attribute(otherAttribute->ID());
                    if (!myAttribute) {
                        myAttribute = new MetadataStructureAttribute(otherAttribute);
                        if (d->datastoreMetadata->createMetadataAttribute(type, myAttribute))
                            mySection->addAttribute(myAttribute);
                        else delete myAttribute;
                    }
                }
            }
            else {
                // Copy Section from other
                mySection = new MetadataStructureSection(otherSection->ID(), otherSection->name(), otherSection->description());
                d->metadataStructure->addSection(type, mySection);
                foreach (MetadataStructureAttribute *otherAttribute, otherSection->attributes()) {
                    MetadataStructureAttribute *myAttribute = new MetadataStructureAttribute(otherAttribute);
                    if (d->datastoreMetadata->createMetadataAttribute(type, myAttribute))
                        mySection->addAttribute(myAttribute);
                    else delete myAttribute;
                }
            }
        }
    }
    d->datastoreMetadata->saveMetadataStructure();
}

void CorpusRepository::importAnnotationStructure(AnnotationStructure *otherStructure)
{
    if (!d->datastoreAnnotations) return;
    if (!d->annotationStructure) return;
    AnnotationStructureLevel *myLevel;
    foreach (AnnotationStructureLevel *otherLevel, otherStructure->levels()) {
        myLevel = d->annotationStructure->level(otherLevel->ID());
        if (myLevel) {
            // Copy over name and description, only if this structure does not already define them
            if (myLevel->name().isEmpty())
                myLevel->setName(otherLevel->name());
            if (myLevel->description().isEmpty())
                myLevel->setDescription(otherLevel->description());
            // Have level in common, check attributes.
            foreach (AnnotationStructureAttribute *otherAttribute, otherLevel->attributes()) {
                AnnotationStructureAttribute *myAttribute = myLevel->attribute(otherAttribute->ID());
                if (!myAttribute) {
                    myAttribute = new AnnotationStructureAttribute(otherAttribute);
                    if (d->datastoreAnnotations->createAnnotationAttribute(myLevel->ID(), myAttribute))
                        myLevel->addAttribute(myAttribute);
                    else delete myAttribute;
                }
            }
        }
        else {
            // Copy level from other
            myLevel = new AnnotationStructureLevel(otherLevel->ID(), otherLevel->levelType(), otherLevel->name(),
                                                   otherLevel->description(), otherLevel->parentLevelID(),
                                                   otherLevel->datatype(), otherLevel->itemOrder(),
                                                   otherLevel->indexed(), otherLevel->nameValueList());
            if (d->datastoreAnnotations->createAnnotationLevel(myLevel)) {
                d->annotationStructure->addLevel(myLevel);
                foreach (AnnotationStructureAttribute *otherAttribute, otherLevel->attributes()) {
                    AnnotationStructureAttribute *myAttribute = new AnnotationStructureAttribute(otherAttribute);
                    if (d->datastoreAnnotations->createAnnotationAttribute(myLevel->ID(), myAttribute))
                        myLevel->addAttribute(myAttribute);
                    else delete myAttribute;
                }
            }
            else delete myLevel;
        }
    }
    d->datastoreAnnotations->saveAnnotationStructure();
}

// ==============================================================================================================================
// Error handling and logging
// ==============================================================================================================================
QString CorpusRepository::lastError() const
{
    return d->lastError;
}

void CorpusRepository::setLastError(const QString &errorMessage)
{
    d->lastError = errorMessage;
    sendLogMessage("ERROR", errorMessage);
}

void CorpusRepository::clearLastError()
{
    d->lastError.clear();
}

void CorpusRepository::sendLogMessage(const QString &category, const QString &message)
{
    emit logMessage(category, message);
}

} // namespace Core
} // namespace Praaline