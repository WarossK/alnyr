#pragma once

#include <srima_src/srima_vertex_buffer.h>

namespace alnyr
{
	class alnyrMeshResource
	{
	protected:
		std::string pipeline_name_;
		srima::srimaVertexBuffer vertex_buffer_;

	public:
		void SetPipelineName(std::string pipeline_name)
		{
			pipeline_name_ = pipeline_name;
		}

		const std::string& GetPipelineName() const
		{
			return pipeline_name_;
		}

		const srima::srimaVertexBuffer& GetVertexBuffer() const
		{
			return vertex_buffer_;
		}
	};
}