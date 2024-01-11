for /r %%v in (*.vert) do (
  D:/Libraries/VulkanSDK_1_3_268/Bin/glslc.exe %%v -o %%v.spv
)

for /r %%f in (*.frag) do (
  D:/Libraries/VulkanSDK_1_3_268/Bin/glslc.exe %%f -o %%f.spv
)

pause