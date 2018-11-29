#pragma once

#include <queue>
#include <alnyr_math.h>
#include <alnyr_object_behavior.h>
#include <alnyr_src/alnyr_positionable_game_object.h>
#include <alnyr_src/alnyr_renderable_game_object.h>

namespace alnyr
{
	class alnyrTransform : public alnyrObjectBehavior
	{
	public:
		Vector3 position;
		Vector3 scale;
		Quaternion rotate;

	private:
		std::unique_ptr<Matrix> local_matrix;

	public:
		alnyrTransform(alnyrPositionableGameObject* game_object)
			: alnyrObjectBehavior(game_object) {}

		Matrix GetWorldMatrix()
		{
			if (!local_matrix)
			{
				auto scl = Matrix::CreateScale(scale);
				auto rot = Matrix::CreateFromQuaternion(rotate);
				auto pos = Matrix::CreateTranslation(position);
				*local_matrix = scl * rot * pos;

				auto parent = static_cast<alnyrPositionableGameObject*>(GetGameObject()->GetParent());
				if (parent)
				{
					return *local_matrix * parent->GetTransform()->GetWorldMatrix();
				}
			}

			return *local_matrix;
		}

		void FlushLocalMatrixCache()
		{
			local_matrix.reset();
		}
	};
}