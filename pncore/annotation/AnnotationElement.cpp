#include "AnnotationElement.h"

namespace Praaline {
namespace Core {

AnnotationElement::AnnotationElement()
{
}

AnnotationElement::AnnotationElement(const RealTime &time, const QString &text) :
    m_time(time), m_text(text)
{
}

AnnotationElement::AnnotationElement(const RealTime &time, const QString &text, const QHash<QString, QVariant> &attributes) :
    m_time(time), m_text(text), m_attributes(attributes)
{
}

} // namespace Core
} // namespace Praaline