-- Copyright © 2008-2016 Pioneer Developers. See AUTHORS.txt for details
-- Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

local Engine = import("Engine")
local Lang = import("Lang")

local Face = import("UI.Game.Face")

local SmallLabeledButton = import("ui/SmallLabeledButton")

local EquipmentTableWidgets = import("EquipmentTableWidgets")

local ui = Engine.ui

local l = Lang.GetResource("ui-core")

local ChatForm = {}

ChatForm.meta = {
	__index = ChatForm,
	class = "ChatForm",
}

function ChatForm.New (chatFunc, removeFunc, closeFunc, ref, tabGroup)
	local form = {
		chatFunc = chatFunc,
		removeFunc = removeFunc,
		closeFunc = closeFunc,
		ref = ref,
		tabGroup = tabGroup,
	}
	setmetatable(form, ChatForm.meta)
	form.chatFunc(form, 0)
	return form
end

function ChatForm:BuildWidget ()
	local box = ui:VBox()
	local hbox = ui:HBox(5)

	if self.title then
		hbox:PackEnd(ui:Expand("HORIZONTAL", ui:MultiLineText(self.title):SetFont("LARGE")))
	end

	if self.face then
		hbox:PackEnd(ui:Align("RIGHT", self.face))
	end

	box:PackEnd(hbox)

	if self.message then
		box:PackEnd(ui:Margin(20, "BOTTOM", ui:Scroller(ui:MultiLineText(self.message))))
	end

	if self.options or self.equipWidgetConfig then

		local scrollBox = ui:VBox()

		if self.options then
			local optionBox = ui:VBox()
			for i = 1,#self.options do
				local option = self.options[i]
				local b = SmallLabeledButton.New(option[1])
				optionBox:PackEnd(b)
				b.button.onClick:Connect(function ()
					if (option[2] == -1) then
						self:Close()
					else
						self.chatFunc(self, option[2])
						ui.layer:SetInnerWidget(self:BuildWidget())
					end
				end)
			end
			scrollBox:PackEnd(optionBox)
		end

		if self.equipWidgetConfig then
			local stationTable, shipTable = EquipmentTableWidgets.Pair(self.equipWidgetConfig)

			scrollBox:PackEnd(
				ui:HBox(10):PackEnd({
					ui:VBox():PackEnd({
						ui:Label(l.AVAILABLE_FOR_PURCHASE):SetFont("HEADING_LARGE"),
						stationTable,
					}),
					ui:VBox():PackEnd({
						ui:Label(l.IN_CARGO_HOLD):SetFont("HEADING_LARGE"),
						shipTable,
					})
				})
			)
		end

		box:PackEnd(ui:Scroller(scrollBox))

	end

	local hangupButton = SmallLabeledButton.New(l.HANG_UP)
	hangupButton.button.onClick:Connect(function () self:Close() end)

	local parent =
		ui:ColorBackground(0,0,0,0.5,
			ui:Grid({10,40,10},{6,30,12}):SetCell(1,1,
				ui:Background(
					ui:VBox(10)
						:PackEnd({ box, hangupButton })
				)
			)
		)

	parent.onVisibilityChanged:Connect(function (visible)
		if (not visible) then
			self:OnClosed()
		end
	end)

	return parent
end

function ChatForm:SetTitle (title)
	self.title = title
end

function ChatForm:SetFace (character)

	local faceFlags = {
		character.female and "FEMALE" or "MALE",
		character.armor and "ARMOUR",
	}

	self.face = Face.New(ui, faceFlags, character.seed):SetHeightLines(5)
end

function ChatForm:ClearFace()
	self.face = nil
end

function ChatForm:SetMessage (message)
	self.message = message
end

function ChatForm:AddOption (text, option)
	self.options = self.options or {}
	table.insert(self.options, { text, option })
end

function ChatForm:Clear ()
	self.title = nil
	self.message = nil
	self.options = nil
	self.tradeFuncs = nil
end

local tradeFuncKeys = { "canTrade", "getStock", "getBuyPrice", "getSellPrice", "onClickBuy", "onClickSell", "bought", "sold" }
function ChatForm:AddGoodsTrader (funcs)
	self.equipWidgetConfig = {
		stationColumns = { "icon", "name", "price", "stock" },
		shipColumns = { "icon", "name", "amount" },
	}

	for i = 1,#tradeFuncKeys do
		local key = tradeFuncKeys[i]
		local fn = funcs[key]
		if fn then
			self.equipWidgetConfig[key] = function (e)
				return fn(self.ref, e)
			end
		end
	end
end

function ChatForm:Close ()
	ui:DropLayer()
end

function ChatForm:GotoPolice ()
	self:Close()
	self.tabGroup:SwitchTo("police")
end

function ChatForm:RemoveAdvertOnClose()
	-- removing it now. has the same effect for the player, without requiring
	-- us to somehow hook the entire view being closed (eg switch back to worldview)
	if self.removeFunc then
		self.removeFunc()
	end
end

function ChatForm:OnClosed()
	self.closeFunc()
end

return ChatForm
