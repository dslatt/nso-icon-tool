
#include "view/collection_grid.hpp"
#include "GenericToolbox.Fs.h"
#include <vector>

using namespace collection;

RecyclerCell::RecyclerCell()
{
  this->inflateFromXMLRes("xml/cells/icon_part_cell_grid.xml");
  this->registerAction("Delete", brls::ControllerButton::BUTTON_X, [this](View *view)
                       {
  auto* recycler = dynamic_cast<RecyclingGrid*>(getParent()->getParent());

  brls::sync([recycler]()
             { brls::Logger::info("{}: in delete", recycler ? "ok" : "null"); });
  if (recycler)
  {
    if(recycler->getDataSource()->onItemAction(recycler, index, brls::ControllerButton::BUTTON_X))
    {
      this->prepareForReuse();
      this->setFocusable(false);
    }
  }
  return true; });
}

RecyclerCell *RecyclerCell::create(std::function<void(std::string)> callback)
{
  auto *cell = new RecyclerCell();
  cell->cb = callback;
  cell->image->allowCaching = false; // disable caching for perf
  return cell;
}

bool DataSource::onItemAction(RecyclingGrid *recycler, size_t index, brls::ControllerButton button)
{
  brls::sync([button, file = files[index]]()
             { brls::Logger::info("button {}, file {}", (int)button, file); });
  if (button == brls::ControllerButton::BUTTON_X)
  {
    if (GenericToolbox::rm(files[index]))
    {
      brls::sync([this, index]()
                { brls::Logger::info("Deleted {}", files[index]); });
      
      files[index] = "none";
      return true;
    }
  }

  return false;
}

DataSource::DataSource(std::vector<std::string> files, std::function<void(std::string)> onSelected, brls::View *parent) : grid::DataSource(files, onSelected, parent) {}

CollectionGrid::CollectionGrid(const std::vector<std::string> &files, std::string title, ImageState &state, std::function<void(std::string)> onSelected, std::function<void(std::string, ImageState &state)> onFocused) : grid::IconPartSelectGrid{}
{
  this->inflateFromXMLRes("xml/views/cell_grid.xml");

  workingImage->setImageFromMemRGBA(state.working.img, state.working.x, state.working.y);
  auto view = workingImage.getView();
  recycler->registerCell("Cell", [view, &state, onFocused]()
                         { return RecyclerCell::create([view, &state, onFocused](std::string path)
                                                       {
                                                         onFocused(path, state);
                                                         view->setImageFromMemRGBA(state.working.img, state.working.x, state.working.y); }); });

  recycler->setDataSource(new collection::DataSource(files, onSelected, this));

  brls::sync([this, title]()
             { getAppletFrame()->setTitle(title); });
}
