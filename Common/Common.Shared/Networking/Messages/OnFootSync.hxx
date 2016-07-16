#pragma once
#include <Networking/SendableEventMessage.hxx>
#include <Math/Math.hxx>
#include <string>

class OnFootSync
{
private:
	Vector3 position;
	Vector3 rotation;
	Vector3 velocity;
	float current_move_blend_ratio;
	float target_move_blend_ratio;
	uint32_t current_weapon_hash;
	bool is_jumping;
	size_t sender;

public:
	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(position, rotation, velocity, current_move_blend_ratio, target_move_blend_ratio, current_weapon_hash, is_jumping, sender);
	}

	UniqueClassId_Declare(OnFootSync, false);

	OnFootSync();

	void GetPosition(Vector3& _position) const;
	void GetRotation(Vector3& _rotation) const;
	void GetVelocity(Vector3& _velocity) const;
	const float GetCurrentMoveBlendRatio() const;
	const float GetTargetMoveBlendRatio() const;
	const uint32_t GetCurrentWeaponHash() const;
	const bool IsJumping() const;
	const size_t GetSender() const;

	void SetPosition(const Vector3& _position);
	void SetRotation(const Vector3& _rotation);
	void SetVelocity(const Vector3& _velocity);
	void SetCurrentMoveBlendRatio(float ratio);
	void SetTargetMoveBlendRatio(float ratio);
	void SetCurrentWeaponHash(uint32_t hash);
	void SetJumping(bool jumping);
	void SetSender(size_t id);
};
