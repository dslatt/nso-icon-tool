#include "view/empty_message.hpp"

EmptyMessage::EmptyMessage(std::string message)
{
  this->inflateFromXMLRes("xml/views/empty_message.xml");
  missingMsg->setFocusable(true);
  missingMsg->setText(message);
}
