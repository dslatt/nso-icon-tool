
#include "view/icon_part_select.hpp"
#include "view/icon_part_select_grid.hpp"
#include <vector>
#include "GenericToolbox.Switch.h"
#include "util/paths.hpp"

RecyclerCell::RecyclerCell()
{
  this->inflateFromXMLRes("xml/cells/icon_part_cell.xml");
}

RecyclerCell *RecyclerCell::create()
{
  auto *cell = new RecyclerCell();
  cell->image->allowCaching = false; // disable caching for perf
  return cell;
}

std::string convertName(std::string name)
{
  std::replace(name.begin(), name.end(), '-', ' ');
  name[0] = toupper(name[0]);

  auto upNext = false;
  for (auto &c : name)
  {
    if (upNext)
      c = toupper(c);
    upNext = (c == ' ');
  }

  return name;
}

RecyclingGridItem *DataSource::cellForRow(RecyclingGrid *recycler, size_t index)
{
  RecyclerCell *item = (RecyclerCell *)recycler->dequeueReusableCell("Cell");
  item->label->setText(convertName(parts[index].name));
  item->image->setImageFromFile(parts[index].icon);
  return item;
}

void DataSource::onItemSelected(RecyclingGrid *recycler, size_t index)
{
  std::vector<std::string> res;

  brls::Logger::info("Selected {} ({})", parts[index].name, parts[index].icon);

  if (parts[index].name == "none")
  {
    if (parent)
    {
      onSelected("");
      parent->dismiss();
      return;
    }
  }

  auto path = GenericToolbox::joinPath(paths::IconCachePath, parts[index].name, subcategory);
  auto files = GenericToolbox::lsFiles(path);
  GenericToolbox::removeEntryIf(files, [](const std::string &entry)
                                { return !GenericToolbox::endsWith(entry, ".png"); });
  for (auto &file : files)
  {
    res.push_back(GenericToolbox::joinPath(path, file));
    brls::Logger::debug("{}", GenericToolbox::joinPath(path, file));
  }
  brls::Logger::debug("total {}", res.size());

  recycler->present(new grid::IconPartSelectGrid(res, convertName(parts[index].name), state, [this](std::string icon)
                                                 {
    onSelected(icon);
    if (parent)
    {
      parent->dismiss();
    } }, onFocused));
}

size_t DataSource::getItemCount()
{
  return parts.size();
}

void DataSource::clearData()
{
  parts.clear();
}

DataSource::DataSource(std::vector<CategoryPart> parts, std::function<void(std::string)> onSelected, std::function<void(std::string, ImageState &state)> onFocused, brls::View *parent, std::string subcategory, ImageState &state) : parent(parent),
                                                                                                                                                                                                                                      subcategory(subcategory),
                                                                                                                                                                                                                                      parts(std::move(parts)), state(state),
                                                                                                                                                                                                                                      onSelected(onSelected),
                                                                                                                                                                                                                                      onFocused(onFocused) {}

IconPartSelect::IconPartSelect(const std::vector<CategoryPart> &files, std::string subcategory, const ImageState &state, std::function<void(std::string)> onSelected, std::function<void(std::string, ImageState &state)> onFocused) : workingState(state)
{
  this->inflateFromXMLRes("xml/views/icon_part_select.xml");
  image->setImageFromMemRGBA(workingState.working.img.get(), workingState.working.x, workingState.working.y);
  recycler->registerCell("Cell", []()
                         { return RecyclerCell::create(); });
  recycler->setDataSource(new DataSource(files, onSelected, onFocused, this, subcategory, workingState));
}
