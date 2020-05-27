# 3D Engine Demo

Here you will find my C++ 3D custom engine demo that will be updated regularly

## Use

Inside the Assets folder is where you will be making edits. If you desire to add more 3D objects into the engine, you need to create .obj and .mtl files (Best used in conjunction with Blender). Place these in inside the Models folder in it's own folder with the models name. It will automatically show up in engine when creating a new object.

Opon starting the engine, a scene called "ArchetypeEditor" is already created. This is where you will create blueprints of objects to place into your world. Once you create an archetype, you need to add a model component for it to render. After a model is created, you can add different combination of shapes or models to create your archetype. Then simply go back to your level and place your objects. 

Different skyboxes can be loaded in by placing images in the Skybox folder. It is a requirement that there are six images in each skybox folder, each containing the names "top", "bottom", "left", "right", "front" and "back". You can also place textures in the texture folder and it will automatically show up in the texture library.
