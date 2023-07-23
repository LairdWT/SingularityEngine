#include "SEDescriptors.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace SE {

// Descriptor Set Layout Builder
SEDescriptorSetLayout::Builder& SEDescriptorSetLayout::Builder::add_binding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count) 
{
	assert(m_Bindings.count(binding) == 0 && "Binding already in use");
	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding = binding;
	layoutBinding.descriptorType = descriptorType;
	layoutBinding.descriptorCount = count;
	layoutBinding.stageFlags = stageFlags;
	m_Bindings[binding] = layoutBinding;
	return *this;
}

std::unique_ptr<SEDescriptorSetLayout> SEDescriptorSetLayout::Builder::build() const 
{
	return std::make_unique<SEDescriptorSetLayout>(m_GraphicsDevice, m_Bindings);
}

// Descriptor Set Layout
SEDescriptorSetLayout::SEDescriptorSetLayout(SEGraphicsDevice& graphicsDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
	: m_GraphicsDevice{ graphicsDevice }, m_Bindings{ bindings } 
{
	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
	for (auto kv : bindings) 
	{
		setLayoutBindings.push_back(kv.second);
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
	descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
	descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

	if (vkCreateDescriptorSetLayout(graphicsDevice.device(), &descriptorSetLayoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

SEDescriptorSetLayout::~SEDescriptorSetLayout() {
	vkDestroyDescriptorSetLayout(m_GraphicsDevice.device(), m_DescriptorSetLayout, nullptr);
}

// Descriptor Pool Builder
SEDescriptorPool::Builder& SEDescriptorPool::Builder::add_pool_size(
	VkDescriptorType descriptorType, uint32_t count) {
	m_PoolSizes.push_back({ descriptorType, count });
	return *this;
}

SEDescriptorPool::Builder& SEDescriptorPool::Builder::set_pool_flags(	VkDescriptorPoolCreateFlags flags) 
{
	m_PolFlags = flags;
	return *this;
}

SEDescriptorPool::Builder& SEDescriptorPool::Builder::set_max_sets(uint32_t count) 
{
	m_MaxSets = count;
	return *this;
}

std::unique_ptr<SEDescriptorPool> SEDescriptorPool::Builder::build() const 
{
	return std::make_unique<SEDescriptorPool>(m_GraphicsDevice, m_MaxSets, m_PolFlags, m_PoolSizes);
}

// Descriptor Pool
SEDescriptorPool::SEDescriptorPool(SEGraphicsDevice& graphicsDevice, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes)
	: m_GraphicsDevice{ graphicsDevice } 
{
	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolInfo.pPoolSizes = poolSizes.data();
	descriptorPoolInfo.maxSets = maxSets;
	descriptorPoolInfo.flags = poolFlags;

	if (vkCreateDescriptorPool(graphicsDevice.device(), &descriptorPoolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

SEDescriptorPool::~SEDescriptorPool() 
{
	vkDestroyDescriptorPool(m_GraphicsDevice.device(), m_DescriptorPool, nullptr);
}

bool SEDescriptorPool::allocate_descriptor_set(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const 
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;
	allocInfo.pSetLayouts = &descriptorSetLayout;
	allocInfo.descriptorSetCount = 1;

	// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
	// a new pool whenever an old pool fills up. But this is beyond our current scope
	if (vkAllocateDescriptorSets(m_GraphicsDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS) 
	{
		return false;
	}
	return true;
}

void SEDescriptorPool::free_descriptors(std::vector<VkDescriptorSet>& descriptors) const 
{
	vkFreeDescriptorSets(m_GraphicsDevice.device(),	m_DescriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
}

void SEDescriptorPool::reset_pool() 
{
	vkResetDescriptorPool(m_GraphicsDevice.device(), m_DescriptorPool, 0);
}

// Descriptor Writer
SEDescriptorWriter::SEDescriptorWriter(SEDescriptorSetLayout& setLayout, SEDescriptorPool& pool)
	: m_SetLayout{ setLayout }, m_DescriptorPool{ pool } 
{
	// TODO: This is a bit of a hack, but it works for now.
}

SEDescriptorWriter& SEDescriptorWriter::write_buffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo) 
{
	assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

	auto& bindingDescription = m_SetLayout.m_Bindings[binding];

	assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType = bindingDescription.descriptorType;
	write.dstBinding = binding;
	write.pBufferInfo = bufferInfo;
	write.descriptorCount = 1;

	m_DescriptorWrites.push_back(write);
	return *this;
}

SEDescriptorWriter& SEDescriptorWriter::write_image(uint32_t binding, VkDescriptorImageInfo* imageInfo) 
{
	assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

	auto& bindingDescription = m_SetLayout.m_Bindings[binding];

	assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType = bindingDescription.descriptorType;
	write.dstBinding = binding;
	write.pImageInfo = imageInfo;
	write.descriptorCount = 1;

	m_DescriptorWrites.push_back(write);
	return *this;
}

bool SEDescriptorWriter::build(VkDescriptorSet& set) 
{
	bool success = m_DescriptorPool.allocate_descriptor_set(m_SetLayout.get_descriptor_set_layout(), set);
	if (!success) 
	{
		return false;
	}
	overwrite(set);
	return true;
}

void SEDescriptorWriter::overwrite(VkDescriptorSet& set) 
{
	for (auto& write : m_DescriptorWrites) 
	{
		write.dstSet = set;
	}
	if (m_DescriptorWrites.size() > std::numeric_limits<uint32_t>::max()) 
	{
		throw std::runtime_error("Too many descriptor writes");
	}
	vkUpdateDescriptorSets(m_DescriptorPool.m_GraphicsDevice.device(), static_cast<uint32_t>(m_DescriptorWrites.size()), m_DescriptorWrites.data(), 0, nullptr);
}

}  // namespace SE