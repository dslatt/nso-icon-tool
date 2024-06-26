#pragma once

#include "view/icon_part_select_grid.hpp"

namespace collection {

  class RecyclerCell
      : public grid::RecyclerCell
  {
  public:
    RecyclerCell();
    static RecyclerCell *create(std::function<void(std::string)> cb);
  };

  class DataSource
      : public grid::DataSource
  {
  public:
    DataSource(std::vector<std::string> files, std::function<void(std::string)> onSelected, brls::View *parent);
    bool onItemAction(RecyclingGrid *recycler, size_t index, brls::ControllerButton button) override;
  };

  class CollectionGrid : public brls::Box
  {
  public:
    CollectionGrid(const std::vector<std::string> &files, std::string title, ImageState &state, std::function<void(std::string)> onSelected, std::function<void(std::string, ImageState &state)> onFocused);

  private:
    BRLS_BIND(RecyclingGrid, recycler, "recycler");
    BRLS_BIND(brls::Image, workingImage, "image");
  };

}
