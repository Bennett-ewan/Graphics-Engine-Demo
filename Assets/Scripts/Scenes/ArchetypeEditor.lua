-- Objects

local selected_object

function ArchetypeEditor:Load()

end

function ArchetypeEditor:Init()
    
end

function ArchetypeEditor:Update(dt)
	Editor.Archetype_Editor()
	selected_object = Editor.Selected_Object()
	if selected_object ~= nil then
		selected_object:Active(true)
		selected_object:Tick(dt)
		selected_object:Active(false)
	end
end

function ArchetypeEditor:Draw()
	if selected_object ~= nil then
		selected_object:Active(true)
		selected_object:Draw()
		selected_object:Active(false)
	end
end

function ArchetypeEditor:Shutdown()
    
end

function ArchetypeEditor:Unload()

end
