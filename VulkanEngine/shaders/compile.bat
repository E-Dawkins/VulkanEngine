for /r %%i in (*.vert) do (
  D:/Libraries/VulkanSDK_1_3_268/Bin/glslc.exe %%i -o %%i.spv
)

for /r %%i in (*.frag) do (
  D:/Libraries/VulkanSDK_1_3_268/Bin/glslc.exe %%i -o %%i.spv
)

pause