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

---@class sol.DimensionLimitUnit
---@field POINT integer
---@field PERCENT integer
---@field MAX_CONTENT integer
---@field FIT_CONTENT integer
---@field STRETCH integer

---@class sol.DimensionLimit
---@field unit integer
---@field value integer

---@class sol.GapGutterMap
---@field column sol.Dimension?
---@field row sol.Dimension?
---@field all sol.Dimension?

---@class sol.PositionMap
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
---@field flexDirection sol.FlexDirection?
---@field flexWrap sol.FlexWrap?
---@field contentAlignment sol.ContentAlignment?
---@field contentJustification sol.ContentJustification?
---@field itemsAlignment sol.ItemAlignment?
---@field selfAlignment sol.ItemAlignment?
---@field aspectRatio number?
---@field displayMode sol.DisplayMode?
---@field gap sol.GapGutterMap?
---@field position sol.PositionMap?
---@field positionType sol.PositionType?

---@class sol.Node
local __node

---@param _type integer
---@see sol.PositionType
function __node:setPositionType(_type) end

---@param style sol.Style?
---@return sol.Node
function __node:addNode(style) end
