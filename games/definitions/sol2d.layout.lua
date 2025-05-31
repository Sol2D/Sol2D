---@meta

---@class sol.HorizontalTextAlignment
---@field NONE integer
---@field BEGIN integer
---@field CENTER integer
---@field END integer

---@class sol.VerticalTextAlignment
---@field NONE integer
---@field TOP integer
---@field CENTER integer
---@field BOTTOM integer

---@class sol.ContentAlignment
---@field FLEX_START integer
---@field FLEX_END integer
---@field STRETCH integer
---@field CENTER integer
---@field SPACE_BETWEEN integer
---@field SPACE_AROUND integer

---@class sol.ContentJustification
---@field FLEX_START integer
---@field FLEX_END integer
---@field CENTER integer
---@field SPACE_BETWEEN integer
---@field SPACE_AROUND integer
---@field SPACE_EVENLY integer

---@class sol.ItemAlignment
---@field STRETCH integer
---@field FLEX_START integer
---@field FLEX_END integer
---@field CENTER integer
---@field BASELINE integer

---@class sol.DisplayMode
---@field FLEX integer
---@field NONE integer

---@class sol.FlexDirection
---@field COLUMN integer
---@field ROW integer
---@field COLUMN_REVERSE integer
---@field ROW_REVERSE integer

---@class sol.FlexWrap
---@field NONE integer
---@field WRAP integer
---@field WRAP_REVERSE integer

---@class sol.Edge
---@field LEFT integer
---@field TOP integer
---@field RIGHT integer
---@field BOTTOM integer
---@field START integer
---@field END integer
---@field HORIZONTAL integer
---@field VERTICAL integer
---@field ALL integer

---@class sol.GapGutter
---@field COLUMN integer
---@field ROW integer
---@field ALL integer

---@class sol.PositionType
---@field RELATIVE integer
---@field ABSOLUTE integer
---@field STATIC integer

---@class sol.PositionUnit
---@field AUTO integer
---@field POINT integer
---@field PERCENT integer

---@class sol.Position
---@field unit integer
---@field value integer
---@see sol.PositionUnit

---@class sol.DimensionUnit
---@field AUTO integer
---@field POINT integer
---@field PERCENT integer
---@field MAX_CONTENT integer
---@field FIT_CONTENT integer
---@field STRETCH integer

---@class sol.SizeLimitUnit
---@field POINT integer
---@field PERCENT integer
---@field MAX_CONTENT integer
---@field FIT_CONTENT integer
---@field STRETCH integer

---@class sol.Dimension
---@field unit integer
---@field value integer
---@see sol.DimensionUnit

---@class sol.DimensionLimitUnit
---@field POINT integer
---@field PERCENT integer
---@field MAX_CONTENT integer
---@field FIT_CONTENT integer
---@field STRETCH integer

---@class sol.DimensionLimit
---@field unit integer
---@field value integer
---@see sol.DimensionLimitUnit

---@class sol.GapGutterMap
---@field column sol.Dimension?
---@field row sol.Dimension?
---@field all sol.Dimension?

---@class sol.PositionMap
---@field left sol.Position?
---@field top sol.Position?
---@field right sol.Position?
---@field bottom sol.Position?
---@field start sol.Position?
---@field end sol.Position?
---@field horizontal sol.Position?
---@field vertical sol.Position?
---@field all sol.Position?

---@class sol.DimensionMap
---@field left sol.Dimension?
---@field top sol.Dimension?
---@field right sol.Dimension?
---@field bottom sol.Dimension?
---@field start sol.Dimension?
---@field end sol.Dimension?
---@field horizontal sol.Dimension?
---@field vertical sol.Dimension?
---@field all sol.Dimension?

---@class sol.Style
---@field margin sol.Dimension?
---@field padding sol.Dimension?
---@field width sol.Dimension?
---@field height sol.Dimension?
---@field minWidth sol.DimensionLimit?
---@field minHeight sol.DimensionLimit?
---@field maxWidth sol.DimensionLimit?
---@field maxHeight sol.DimensionLimit?
---@field flexBasis sol.Dimension?
---@field flexGrow number?
---@field flexShrink number?
---@field flexDirection integer?
---@field flexWrap integer?
---@field contentAlignment integer?
---@field contentJustification integer?
---@field itemsAlignment integer?
---@field selfAlignment integer?
---@field aspectRatio number?
---@field displayMode integer?
---@field gap sol.GapGutterMap?
---@field position sol.PositionMap?
---@field positionType integer?

---@class sol.Node
local __node

---@param type integer
---@see sol.PositionType
function __node:setPositionType(type) end

---@param edge integer
---@param position sol.Position
---@see sol.Edge
function __node:setPosition(edge, position) end

---@param position_map sol.PositionMap
function __node:setPosition(position_map) end

---@param edge integer
---@param margin sol.Dimension
---@see sol.Edge
function __node:setMargin(edge, margin) end

---@param margin_map sol.DimensionMap
function __node:setMargin(margin_map) end

---@param edge integer
---@param padding sol.Dimension
---@see sol.Edge
function __node:setPadding(edge, padding) end

---@param padding_map sol.DimensionMap
function __node:setPadding(padding_map) end

---@param width sol.Dimension
function __node:setWidth(width) end

---@param height sol.Dimension
function __node:setHeight(height) end

---@param limit sol.DimensionLimit
function __node:setMinWidth(limit) end

---@param limit sol.DimensionLimit
function __node:setMinHeight(limit) end

---@param limit sol.DimensionLimit
function __node:setMaxWidth(limit) end

---@param limit sol.DimensionLimit
function __node:setMaxHeight(limit) end

---@param basis sol.Dimension
function __node:setFlexBasis(basis) end

---@param grow number
function __node:setFlexGrow(grow) end

---@param shrink number
function __node:setFlexShrink(shrink) end

---@param direction integer
---@see sol.FlexDirection
function __node:setFlexDirection(direction) end

---@param wrap integer
---@see sol.FlexWrap
function __node:setFlexWrap(wrap) end

---@param gap_gutter integer
---@param gap sol.Dimension
---@see sol.GapGutter
function __node:setGap(gap_gutter, gap) end

---@param gap_gutter_map sol.GapGutterMap
function __node:setGap(gap_gutter_map) end

---@param alignment integer
---@see sol.ContentAlignment
function __node:setContentAlignment(alignment) end

---@param justification integer
---@see sol.ContentJustification
function __node:setContentJustification(justification) end

---@param alignment integer
---@see sol.ItemAlignment
function __node:setItemsAlignment(alignment) end

---@param alignment integer
---@see sol.ItemAlignment
function __node:setSelfAlignment(alignment) end

---@param ratio number
function __node:setAspectRatio(ratio) end

---@param mode integer
---@see sol.DisplayMode
function __node:setDisplayMode(mode) end

---@param style sol.Style?
---@return sol.Node
function __node:addNode(style) end

---@return number
function __node:getX() end

---@return number
function __node:getY() end

---@return number
function __node:getWidth() end

---@return number
function __node:getHeight() end

---@return sol.Node[]
function __node:getChildren() end
