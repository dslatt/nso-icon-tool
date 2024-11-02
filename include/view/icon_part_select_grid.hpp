#pragma once

#include <borealis.hpp>
#include <borealis/core/event.hpp>

#include "state/image_state.hpp"
#include "view/recycling_grid.hpp"

namespace grid {

typedef brls::Event<std::string> PartSelectEvent;

class RecyclerCell : public RecyclingGridItem {
public:
  RecyclerCell();

  BRLS_BIND(brls::Image, image, "image");

  virtual void prepareForReuse() override
  {
    image->clear();
    img.clear();
  }

  virtual void cacheForReuse() override
  {
    image->clear();
    img.clear();
  }

  virtual void onFocusGained() override;

  static RecyclerCell* create(std::function<void(std::string)> cb);
  std::string img = "";
  std::function<void(std::string)> cb;
};

class DataSource : public RecyclingGridDataSource {
public:
  DataSource(std::vector<std::string> files, std::function<void(std::string)> onSelected, brls::View* parent);

  RecyclingGridItem* cellForRow(RecyclingGrid* recycler, size_t index) override;

  void onItemSelected(RecyclingGrid* recycler, size_t index) override;

  void clearData() override;

  size_t getItemCount() override;

  std::function<void(std::string)> onSelected;
  std::vector<std::string> files;
  brls::View* parent = nullptr;
};

class IconPartSelectGrid : public brls::Box {
public:
  IconPartSelectGrid(const std::vector<std::string>& files, std::string title, ImageState& state,
      std::function<void(std::string)> onSelected, std::function<void(std::string, ImageState& state)> onFocused);

private:
  BRLS_BIND(RecyclingGrid, recycler, "recycler");
  BRLS_BIND(brls::Image, workingImage, "image");
};

}