
#include "view/icon_part_select_grid.hpp"
#include <vector>

using namespace grid;

RecyclerCell::RecyclerCell()
{
  this->inflateFromXMLRes("xml/cells/icon_part_cell_grid.xml");
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
  brls::Logger::debug("image: {}", files[index]);
  item->image->setImageFromFile(files[index]);
  item->img = files[index];
  return item;
}

void DataSource::onItemSelected(RecyclingGrid *recycler, size_t index)
{
  brls::Logger::info("Selected {}", files[index]);
  onSelected(files[index]);

  if (parent)
  {
    parent->dismiss();
  }
}

size_t DataSource::getItemCount()
{
  return files.size();
}

void DataSource::clearData()
{
  files.clear();
}

DataSource::DataSource(std::vector<std::string> files, std::function<void(std::string)> onSelected, brls::View *parent) : onSelected(onSelected), parent(parent), files(std::move(files)) {}

IconPartSelectGrid::IconPartSelectGrid(const std::vector<std::string> &files, std::string title, ImageState &state, std::function<void(std::string)> onSelected, std::function<void(std::string, ImageState &state)> onFocused)
{
  this->inflateFromXMLRes("xml/views/icon_part_select_grid.xml");

  workingImage->setImageFromMemRGBA(state.working.data.get(), state.working.x, state.working.y);
  auto view = workingImage.getView();
  recycler->registerCell("Cell", [view, &state, onFocused]()
                         { return RecyclerCell::create([view, &state, onFocused](std::string path)
                                                       {
                                                         onFocused(path, state);
                                                         view->setImageFromMemRGBA(state.working.data.get(), state.working.x, state.working.y);
                                                       }); });

  recycler->setDataSource(new DataSource(files, onSelected, this));

  brls::sync([this, title]()
             { getAppletFrame()->setTitle(title); });
}
