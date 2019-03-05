# TODO list

## Render

- [ ] Remove static linkage of *vulkan-1.lib* and switch to function pointers (this may ruin fancy *vulkan.hpp* usage)
- [ ] Switch from exceptions provided by *vulkan.hpp* to ret codes check

### Front end

#### Resource Manager

- [ ] Remove resource reference counting from `Create*` (shared ptr already do this)
- [ ] Shader compiler lacks constant reflection (consider `spirv-cross`)
- [ ] Blender compiler uses TL as default pipeline topology

### Back end

- [ ] Add statistics for buffers utilization (see ``stats_manager``)

### Memory allocation

- [x] Adopt VMA to do this dirty job. But first need to find all possible use cases to adjust optimal allocation policy
- [ ] Make VMA C++ friendly (hpp wrapper)

### Device

- [ ] ``SelectGpu``: check each GPU for WSI support (currently only first found is used). Nice to have scoring system.
- [ ] Try structures chain to obtain device driver properties
- [ ] ``CreateLogicalDevice`` handle a case when graphics and presentation queues came from different families
- [ ] By now ``CreateSwapchain`` doesn't care of presentation images amount and takes min value. However, it's nice to decide what kind of buffering scheme will be used (and can be supported)
- [ ] Move ``stats_manager`` into backend. It has no relations with device
- [x] ``CreateSwapchain`` : check for alpha compositing

## Application

- [ ] Engine and application name strings should be defined project wide
- [ ] Consider dynamic renders enumerator
- [ ] Need to unload unused renders
- [ ] `CGameFont` has `dxFontRender` in friends. Need to enlarge friend zone or find a smarter way to access protected members

## Issues

- [x] GetSwapChainStatus link error (opened ticket #[836](https://vulkan.lunarg.com/issue/view/5c53595f6631513491ccd8f3) )
  There is no guarantee (according to the spec) from SDK side to support extensions symbols in static library. More over this particular function is a part of `VK_KHR_shared_presentable_image`
