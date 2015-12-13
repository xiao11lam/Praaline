#include "speakertimeline.h"

SpeakerTimeline::SpeakerTimeline(QMap<QString, QPointer<AnnotationTierGroup> > &tiers, const QString &levelID, QObject *parent) :
    QObject(parent), m_tiers(tiers), m_levelID(levelID)
{
}

SpeakerTimeline::~SpeakerTimeline()
{
}
