#pragma once

#include <borealis.hpp>
#include "view/recycling_grid.hpp"
#include "state/image_state.hpp"

#include <borealis/core/event.hpp>

typedef brls::Event<std::string> PartSelectEvent;

struct CategoryPart
{
  std::string name;
  std::string icon;
  CategoryPart(std::string name, std::string icon) : name(name), icon(icon) {}
};

class RecyclerCell
    : public RecyclingGridItem
{
public:
  RecyclerCell();

  BRLS_BIND(brls::Rectangle, accent, "brls/sidebar/item_accent");
  BRLS_BIND(brls::Label, label, "title");
  BRLS_BIND(brls::Image, image, "image");

  virtual void prepareForReuse() override
  {
    image->clear();
  }

  virtual void cacheForReuse() override
  {
    image->clear();
  }

  static RecyclerCell *create();
};

class DataSource
    : public RecyclingGridDataSource
{
public:
  DataSource(std::vector<CategoryPart> parts, std::function<void(std::string)> onSelected, std::function<void(std::string, ImageState &state)> onFocused, brls::View *parent, std::string subcategory, ImageState &state);

  RecyclingGridItem *cellForRow(RecyclingGrid *recycler, size_t index) override;

  void onItemSelected(RecyclingGrid *recycler, size_t index) override;

  void clearData() override;

  size_t getItemCount() override;

  std::function<void(std::string)> onSelected;
  std::function<void(std::string, ImageState &state)> onFocused;
  brls::View *parent = nullptr;
  std::vector<CategoryPart> parts;
  std::string subcategory;
  ImageState &state;
};

class IconPartSelect : public brls::Box
{
public:
  IconPartSelect(const std::vector<CategoryPart> &files, std::string subcategory, const ImageState &state, std::function<void(std::string)> onSelected, std::function<void(std::string, ImageState &state)> onFocused);

private:
  BRLS_BIND(RecyclingGrid, recycler, "recycler");
  BRLS_BIND(brls::Image, image, "image");
  ImageState workingState;
};
