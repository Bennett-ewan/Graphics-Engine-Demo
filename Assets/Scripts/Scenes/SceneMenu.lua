-- Objects


function SceneMenu:Load()
	
end

function SceneMenu:Init()
    Editor.Enable_Editor(false)
end

function SceneMenu:Update(dt)
	Editor.Scene_Selector()
end

function SceneMenu:Draw()

end

function SceneMenu:Shutdown()
    Editor.Enable_Editor(true)
end

function SceneMenu:Unload()

end
