
#include "view/collection_grid.hpp"
#include "GenericToolbox.Fs.h"
#include <vector>

using namespace brls::literals; // for _i18n

using namespace collection;

RecyclerCell::RecyclerCell()
{
  this->inflateFromXMLRes("xml/cells/icon_part_cell_grid.xml");
  this->registerAction("hints/select"_i18n, brls::BUTTON_X, [this](View *view)
  {
  auto* recycler = dynamic_cast<RecyclingGrid*>(getParent()->getParent());

  brls::sync([recycler]()
             { brls::Logger::info("{}: in delete", recycler ? "ok" : "null"); });
  if (recycler)
  {
    auto res = recycler->getDataSource()->onItemAction(recycler, index, brls::ControllerButton::BUTTON_X);
    recycler->getDataSource()->updateCell(this, index);
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

void RecyclerCell::onFocusGained()
{
  Box::onFocusGained();
  cb(img);
}

RecyclingGridItem *DataSource::cellForRow(RecyclingGrid *recycler, size_t index)
{
  RecyclerCell *item = (RecyclerCell *)recycler->dequeueReusableCell("Cell");
  brls::Logger::debug("image: {}", items[index].file);

  if (items[index].image.img == nullptr) {
    items[index].image = Image(items[index].file);
  }

  item->image->setImageFromMemRGBA(items[index].image.img, items[index].image.x, items[index].image.y);
  item->img = items[index].file;
  return item;
}

void DataSource::onItemSelected(RecyclingGrid *recycler, size_t index)
{
  brls::Logger::info("Selected {}", items[index].file);
  if (!items[index].file.empty()) {
    onSelected(items[index].file);

    if (parent)
    {
      parent->dismiss();
    }
  }
}

void DataSource::updateCell(RecyclingGridItem* item, size_t index)
{
  auto cell = dynamic_cast<RecyclerCell*>(item);
  if (cell) {
    cell->image->setImageFromMemRGBA(items[index].image.img, items[index].image.x, items[index].image.y);
  }  
}

size_t DataSource::getItemCount()
{
  return items.size();
}

void DataSource::clearData()
{
  items.clear();
}

bool DataSource::onItemAction(RecyclingGrid *recycler, size_t index, brls::ControllerButton button)
{
  brls::sync([button, file = items[index].file]()
             { brls::Logger::info("button {}, file {}", (int)button, file); });
  if (button == brls::ControllerButton::BUTTON_X)
  {
    auto select = !items[index].selected;
    items[index].selected = select;
    if (select) { items[index].image.applyAlpha(0.15f); }
    else { items[index].image = Image(items[index].file); }

    auto anySelected = std::any_of(items.begin(), items.end(), [](CollectionItem& v){
      return v.selected;
    });

    auto* grid = dynamic_cast<CollectionGrid*>(parent);
    if (grid) {
      grid->confirmDelete->setVisibility(anySelected ? brls::Visibility::VISIBLE : brls::Visibility::INVISIBLE);
    }
  }

  return select;
}

DataSource::DataSource(std::vector<CollectionItem> items, std::function<void(std::string)> onSelected, brls::View *parent) : onSelected(onSelected), parent(parent), items(std::move(items)) {}

void DataSource::deleteSelected()
{
  for(auto& item : items) {
    if (item.selected) {
      if (GenericToolbox::rm(item.file))
      {
        brls::sync([this, file = item.file]()
                  { brls::Logger::info("Deleted {}", file); });
      }
    }
  }
}

CollectionGrid::CollectionGrid(const std::vector<std::string> &files, std::string title, ImageState &state, std::function<void(std::string)> onSelected, std::function<void(std::string, ImageState &state)> onFocused)
{
  this->inflateFromXMLRes("xml/views/cell_grid.xml");

  std::vector<CollectionItem> items; 
  for (auto& file : files) {
   items.push_back(CollectionItem{file, Image{}, false}); 
  }

  workingImage->setImageFromMemRGBA(state.working.img, state.working.x, state.working.y);
  recycler->registerCell("Cell", [view = workingImage.getView(), &state, onFocused]()
                         { return RecyclerCell::create([view, &state, onFocused](std::string path)
                                                       {
                                                         onFocused(path, state);
                                                         view->setImageFromMemRGBA(state.working.img, state.working.x, state.working.y); }); });
  auto* data = new collection::DataSource(std::move(items), onSelected, this);
  recycler->setDataSource(data);

  confirmDelete->registerClickAction([this, data](...) {
    data->deleteSelected();
    if (parent) parent->dismiss();
    return true;
  });

  confirmDelete->setVisibility(brls::Visibility::INVISIBLE);

  brls::sync([this, title]()
             { getAppletFrame()->setTitle(title); });
}
