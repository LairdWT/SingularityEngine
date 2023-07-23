#pragma once

#include "SERendering/SEGraphicsDevice/SEGraphicsDevice.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace SE {

class SEDescriptorSetLayout 
{

public:

	class Builder 
	{

	public:
		Builder(SEGraphicsDevice& graphicsDevice) : m_GraphicsDevice{ graphicsDevice } {}

		Builder& add_binding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);
		std::unique_ptr<SEDescriptorSetLayout> build() const;

	private:

		SEGraphicsDevice& m_GraphicsDevice;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings{};
	}; // end Builder

	SEDescriptorSetLayout(SEGraphicsDevice& graphicsDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
	~SEDescriptorSetLayout();
	SEDescriptorSetLayout(const SEDescriptorSetLayout&) = delete;
	SEDescriptorSetLayout& operator=(const SEDescriptorSetLayout&) = delete;

	VkDescriptorSetLayout get_descriptor_set_layout() const { return m_DescriptorSetLayout; }

private:
	SEGraphicsDevice& m_GraphicsDevice;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;

	friend class SEDescriptorWriter;
};

class SEDescriptorPool 
{
public:
	class Builder 
	{
	public:
		Builder(SEGraphicsDevice& graphicsDevice) : m_GraphicsDevice{ graphicsDevice } {}

		Builder& add_pool_size(VkDescriptorType descriptorType, uint32_t count);
		Builder& set_pool_flags(VkDescriptorPoolCreateFlags flags);
		Builder& set_max_sets(uint32_t count);
		std::unique_ptr<SEDescriptorPool> build() const;

	private:
		SEGraphicsDevice& m_GraphicsDevice;
		std::vector<VkDescriptorPoolSize> m_PoolSizes{};
		uint32_t m_MaxSets = 1000;
		VkDescriptorPoolCreateFlags m_PolFlags = 0;
	}; // end Builder	

	SEDescriptorPool(SEGraphicsDevice& graphicsDevice, uint32_t maxSets,	VkDescriptorPoolCreateFlags poolFlags,const std::vector<VkDescriptorPoolSize>& poolSizes);
	~SEDescriptorPool();
	SEDescriptorPool(const SEDescriptorPool&) = delete;
	SEDescriptorPool& operator=(const SEDescriptorPool&) = delete;

	bool allocate_descriptor_set(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

	void free_descriptors(std::vector<VkDescriptorSet>& descriptors) const;

	void reset_pool();

private:
	SEGraphicsDevice& m_GraphicsDevice;
	VkDescriptorPool m_DescriptorPool;

	friend class SEDescriptorWriter;
};

class SEDescriptorWriter 
{
public:
	SEDescriptorWriter(SEDescriptorSetLayout& setLayout, SEDescriptorPool& pool);

	SEDescriptorWriter& write_buffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
	SEDescriptorWriter& write_image(uint32_t binding, VkDescriptorImageInfo* imageInfo);

	bool build(VkDescriptorSet& set);
	void overwrite(VkDescriptorSet& set);

private:
	SEDescriptorSetLayout& m_SetLayout;
	SEDescriptorPool& m_DescriptorPool;
	std::vector<VkWriteDescriptorSet> m_DescriptorWrites;
};

}  // namespace SE