#include <QDebug>
#include <QSqlError>
#include "sqlserialisercorpus.h"
#include "sqlserialisermetadatastructure.h"
#include "sqlschemaproxymetadata.h"

#include "sqlmetadatadatastore.h"

SQLMetadataDatastore::SQLMetadataDatastore(QPointer<MetadataStructure> structure, QObject *parent) :
    AbstractMetadataDatastore(parent), m_structure(structure)
{
}

SQLMetadataDatastore::~SQLMetadataDatastore()
{
    closeDatastore();
}

bool SQLMetadataDatastore::createDatastore(const DatastoreInfo &info)
{
    if (m_database.isOpen()) m_database.close();
    m_database = QSqlDatabase::addDatabase(info.driver, info.datasource + "_metadata");
    m_database.setHostName(info.hostname);
    m_database.setDatabaseName(info.datasource);
    m_database.setUserName(info.username);
    m_database.setPassword(info.password);
    if (!m_database.open()) {
        m_lastError = m_database.lastError().text();
        qDebug() << m_lastError;
        return false;
    }
    // Initialise database
    SQLSerialiserMetadataStructure::initialiseMetadataStructureTables(m_database);
    SQLSchemaProxyMetadata::createSchema(m_structure, m_database);
    return true;
}

bool SQLMetadataDatastore::openDatastore(const DatastoreInfo &info)
{
    if (m_database.isOpen()) m_database.close();
    m_database = QSqlDatabase::addDatabase(info.driver, info.datasource + "_metadata");
    m_database.setHostName(info.hostname);
    m_database.setDatabaseName(info.datasource);
    m_database.setUserName(info.username);
    m_database.setPassword(info.password);
    if (!m_database.open()) {
        m_lastError = m_database.lastError().text();
        qDebug() << m_lastError;
        return false;
    }
    return true;
}

bool SQLMetadataDatastore::closeDatastore()
{
    if (m_database.isOpen())
        m_database.close();
    return true;
}

bool SQLMetadataDatastore::createMetadataAttribute(CorpusObject::Type type, QPointer<MetadataStructureAttribute> newAttribute)
{
    return SQLSchemaProxyMetadata::createMetadataAttribute(type, newAttribute, m_database);
}

bool SQLMetadataDatastore::renameMetadataAttribute(CorpusObject::Type type, QString attributeID, QString newAttributeID)
{
    return SQLSchemaProxyMetadata::renameMetadataAttribute(type, attributeID, newAttributeID, m_database);
}

bool SQLMetadataDatastore::deleteMetadataAttribute(CorpusObject::Type type, QString attributeID)
{
    return SQLSchemaProxyMetadata::deleteMetadataAttribute(type, attributeID, m_database);
}

bool SQLMetadataDatastore::loadMetadataStructure()
{
    return SQLSerialiserMetadataStructure::read(m_structure, m_database);
}

bool SQLMetadataDatastore::saveMetadataStructure()
{
    return SQLSerialiserMetadataStructure::write(m_structure, m_database);
}

bool SQLMetadataDatastore::loadCorpus(Corpus *corpus)
{
    return SQLSerialiserCorpus::loadCorpus(corpus, m_database);
}

bool SQLMetadataDatastore::saveCorpus(Corpus *corpus)
{
    return SQLSerialiserCorpus::saveCorpus(corpus, m_database);
}

