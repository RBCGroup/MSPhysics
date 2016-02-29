#include "msp_joint_spring.h"

/*
 ///////////////////////////////////////////////////////////////////////////////
  Variables
 ///////////////////////////////////////////////////////////////////////////////
*/

const dFloat MSNewton::Spring::DEFAULT_MIN = -10.0f;
const dFloat MSNewton::Spring::DEFAULT_MAX = 10.0f;
const dFloat MSNewton::Spring::DEFAULT_STIFF = 40.0f;
const dFloat MSNewton::Spring::DEFAULT_DAMP = 10.0f;
const bool MSNewton::Spring::DEFAULT_LIMITS_ENABLED = false;
const dFloat MSNewton::Spring::DEFAULT_START_POSITION = 0.0f;
const dFloat MSNewton::Spring::DEFAULT_CONTROLLER = 1.0f;


/*
 ///////////////////////////////////////////////////////////////////////////////
  Callback Functions
 ///////////////////////////////////////////////////////////////////////////////
*/

void MSNewton::Spring::submit_constraints(const NewtonJoint* joint, dgFloat32 timestep, int thread_index) {
	JointData* joint_data = (JointData*)NewtonJointGetUserData(joint);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;

	// Calculate position of pivot points and Jacobian direction vectors in global space.
	dMatrix matrix0;
	dMatrix matrix1;
	Joint::c_calculate_global_matrix(joint_data, matrix0, matrix1);

	dVector pos0(matrix0.m_posit);
	dVector pos1(matrix1.m_posit + matrix1.m_right.Scale((pos0 - matrix1.m_posit) % matrix1.m_right));

	// Calculate position, velocity, and acceleration
	dFloat last_pos = cj_data->cur_pos;
	dFloat last_vel = cj_data->cur_vel;
	cj_data->cur_pos = matrix1.UntransformVector(matrix0.m_posit).m_z;
	cj_data->cur_vel = (cj_data->cur_pos - last_pos) / timestep;
	cj_data->cur_accel = (cj_data->cur_vel - last_vel) / timestep;
	dFloat cur_pos = cj_data->cur_pos - cj_data->start_pos * cj_data->controller;

	// Restrict movement on axes perpendicular to the pin direction.
	NewtonUserJointAddLinearRow(joint, &pos0[0], &pos1[0], &matrix0.m_front[0]);
	if (joint_data->ctype == CT_FLEXIBLE)
		NewtonUserJointSetRowSpringDamperAcceleration(joint, Joint::LINEAR_STIFF, Joint::LINEAR_DAMP);
	else if (joint_data->ctype == CT_ROBUST)
		NewtonUserJointSetRowAcceleration(joint, NewtonUserCalculateRowZeroAccelaration(joint));
	NewtonUserJointSetRowStiffness(joint, joint_data->stiffness);

	NewtonUserJointAddLinearRow(joint, &pos0[0], &pos1[0], &matrix0.m_up[0]);
	if (joint_data->ctype == CT_FLEXIBLE)
		NewtonUserJointSetRowSpringDamperAcceleration(joint, Joint::LINEAR_STIFF, Joint::LINEAR_DAMP);
	else if (joint_data->ctype == CT_ROBUST)
		NewtonUserJointSetRowAcceleration(joint, NewtonUserCalculateRowZeroAccelaration(joint));
	NewtonUserJointSetRowStiffness(joint, joint_data->stiffness);

	// Add three angular rows to restrict rotation around all axes.
	/*NewtonUserJointAddAngularRow(joint, Joint::c_calculate_angle(matrix0.m_right, matrix1.m_right, matrix0.m_front), &matrix0.m_front[0]);
	if (joint_data->ctype == CT_FLEXIBLE)
		NewtonUserJointSetRowSpringDamperAcceleration(joint, Joint::ANGULAR_STIFF, Joint::ANGULAR_DAMP);
	else if (joint_data->ctype == CT_ROBUST)
		NewtonUserJointSetRowAcceleration(joint, NewtonUserCalculateRowZeroAccelaration(joint));
	NewtonUserJointSetRowStiffness(joint, joint_data->stiffness);

	NewtonUserJointAddAngularRow(joint, Joint::c_calculate_angle(matrix0.m_right, matrix1.m_right, matrix0.m_up), &matrix0.m_up[0]);
	if (joint_data->ctype == CT_FLEXIBLE)
		NewtonUserJointSetRowSpringDamperAcceleration(joint, Joint::ANGULAR_STIFF, Joint::ANGULAR_DAMP);
	else if (joint_data->ctype == CT_ROBUST)
		NewtonUserJointSetRowAcceleration(joint, NewtonUserCalculateRowZeroAccelaration(joint));
	NewtonUserJointSetRowStiffness(joint, joint_data->stiffness);

	NewtonUserJointAddAngularRow(joint, Joint::c_calculate_angle(matrix0.m_front, matrix1.m_front, matrix0.m_right), &matrix0.m_right[0]);
	if (joint_data->ctype == CT_FLEXIBLE)
		NewtonUserJointSetRowSpringDamperAcceleration(joint, Joint::ANGULAR_STIFF, Joint::ANGULAR_DAMP);
	else if (joint_data->ctype == CT_ROBUST)
		NewtonUserJointSetRowAcceleration(joint, NewtonUserCalculateRowZeroAccelaration(joint));
	NewtonUserJointSetRowStiffness(joint, joint_data->stiffness);*/

	// Add three more rows for a more robust angular constraint.
	// Get a point along the ping axis at some reasonable large distance from the pivot
	dVector q0(pos0 + matrix0.m_right.Scale(MIN_JOINT_PIN_LENGTH));
	dVector q1(pos1 + matrix1.m_right.Scale(MIN_JOINT_PIN_LENGTH));

	// Add two constraints row perpendicular to the pin
	NewtonUserJointAddLinearRow(joint, &q0[0], &q1[0], &matrix0.m_front[0]);
	if (joint_data->ctype == CT_FLEXIBLE)
		NewtonUserJointSetRowSpringDamperAcceleration(joint, Joint::LINEAR_STIFF, Joint::LINEAR_DAMP);
	else if (joint_data->ctype == CT_ROBUST)
		NewtonUserJointSetRowAcceleration(joint, NewtonUserCalculateRowZeroAccelaration(joint));
	NewtonUserJointSetRowStiffness(joint, joint_data->stiffness);

	NewtonUserJointAddLinearRow(joint, &q0[0], &q1[0], &matrix0.m_up[0]);
	if (joint_data->ctype == CT_FLEXIBLE)
		NewtonUserJointSetRowSpringDamperAcceleration(joint, Joint::LINEAR_STIFF, Joint::LINEAR_DAMP);
	else if (joint_data->ctype == CT_ROBUST)
		NewtonUserJointSetRowAcceleration(joint, NewtonUserCalculateRowZeroAccelaration(joint));
	NewtonUserJointSetRowStiffness(joint, joint_data->stiffness);

	// Get a point along the ping axis at some reasonable large distance from the pivot
	dVector r0(pos0 + matrix0.m_front.Scale(MIN_JOINT_PIN_LENGTH));
	dVector r1(pos1 + matrix1.m_front.Scale(MIN_JOINT_PIN_LENGTH));

	// Add one constraint row perpendicular to the pin
	NewtonUserJointAddLinearRow(joint, &r0[0], &r1[0], &matrix0.m_up[0]);
	if (joint_data->ctype == CT_FLEXIBLE)
		NewtonUserJointSetRowSpringDamperAcceleration(joint, Joint::LINEAR_STIFF, Joint::LINEAR_DAMP);
	else if (joint_data->ctype == CT_ROBUST)
		NewtonUserJointSetRowAcceleration(joint, NewtonUserCalculateRowZeroAccelaration(joint));
	NewtonUserJointSetRowStiffness(joint, joint_data->stiffness);

	// Check if need to re-enable limits.
	if (cj_data->temp_disable_limits == true && cur_pos >= cj_data->min && cur_pos <= cj_data->max)
		cj_data->temp_disable_limits = false;

	// Add limits and friction
	if (cj_data->limits_enabled == true && cur_pos < cj_data->min && cj_data->temp_disable_limits == false) {
		const dVector& s0 = matrix0.m_posit;
		dVector s1(s0 + matrix1.m_right.Scale(cj_data->min + Joint::LINEAR_LIMIT_EPSILON - cur_pos));
		NewtonUserJointAddLinearRow(joint, &s0[0], &s1[0], &matrix1.m_right[0]);
		NewtonUserJointSetRowMinimumFriction(joint, 0.0f);
		if (joint_data->ctype == CT_FLEXIBLE)
			NewtonUserJointSetRowSpringDamperAcceleration(joint, Joint::ANGULAR_STIFF, Joint::ANGULAR_DAMP);
		else //if (joint_data->ctype == CT_ROBUST)
			NewtonUserJointSetRowAcceleration(joint, NewtonUserCalculateRowZeroAccelaration(joint));
		NewtonUserJointSetRowStiffness(joint, joint_data->stiffness);
	}
	else if (cj_data->limits_enabled == true && cur_pos > cj_data->max && cj_data->temp_disable_limits == false) {
		const dVector& s0 = matrix0.m_posit;
		dVector s1(s0 + matrix1.m_right.Scale(cj_data->max - Joint::LINEAR_LIMIT_EPSILON - cur_pos));
		NewtonUserJointAddLinearRow(joint, &s0[0], &s1[0], &matrix1.m_right[0]);
		NewtonUserJointSetRowMaximumFriction(joint, 0.0f);
		if (joint_data->ctype == CT_FLEXIBLE)
			NewtonUserJointSetRowSpringDamperAcceleration(joint, Joint::ANGULAR_STIFF, Joint::ANGULAR_DAMP);
		else //if (joint_data->ctype == CT_ROBUST)
			NewtonUserJointSetRowAcceleration(joint, NewtonUserCalculateRowZeroAccelaration(joint));
		NewtonUserJointSetRowStiffness(joint, joint_data->stiffness);
	}
	else {
		const dVector& s0 = matrix0.m_posit;
		dVector s1(s0 + matrix1.m_right.Scale(-cur_pos));
		NewtonUserJointAddLinearRow(joint, &s0[0], &s1[0], &matrix1.m_right[0]);
		NewtonUserJointSetRowSpringDamperAcceleration(joint, cj_data->stiff, cj_data->damp);
		NewtonUserJointSetRowStiffness(joint, joint_data->stiffness * Joint::STIFFNESS_RATIO);
	}
}

void MSNewton::Spring::get_info(const NewtonJoint* const joint, NewtonJointRecord* const info) {
	JointData* joint_data = (JointData*)NewtonJointGetUserData(joint);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;

	info->m_minLinearDof[0] = -0.0f;
	info->m_maxLinearDof[0] = 0.0f;
	info->m_minLinearDof[1] = -0.0f;
	info->m_maxLinearDof[1] = 0.0f;

	if (cj_data->limits_enabled) {
		info->m_minLinearDof[2] = (cj_data->min - cj_data->cur_pos);
		info->m_minLinearDof[2] = (cj_data->max - cj_data->cur_pos);
	}
	else {
		info->m_minLinearDof[2] = -Joint::CUSTOM_LARGE_VALUE;
		info->m_minLinearDof[2] = Joint::CUSTOM_LARGE_VALUE;
	}

	info->m_minAngularDof[0] = -0.0f;
	info->m_maxAngularDof[0] = 0.0f;
	info->m_minAngularDof[1] = -0.0f;
	info->m_maxAngularDof[1] = 0.0f;
	info->m_minAngularDof[2] = -0.0f;
	info->m_maxAngularDof[2] = 0.0f;
}

void MSNewton::Spring::on_destroy(JointData* joint_data) {
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	delete cj_data;
}

void MSNewton::Spring::on_connect(JointData* joint_data) {
}

void MSNewton::Spring::on_disconnect(JointData* joint_data) {
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	cj_data->cur_pos = 0.0f;
	cj_data->cur_vel = 0.0f;
	cj_data->cur_accel = 0.0f;
}


/*
 ///////////////////////////////////////////////////////////////////////////////
  Ruby Functions
 ///////////////////////////////////////////////////////////////////////////////
*/

VALUE MSNewton::Spring::is_valid(VALUE self, VALUE v_joint) {
	JointData* address = (JointData*)Util::value_to_ll(v_joint);
	bool valid = Util::is_joint_valid(address);
	if (valid && address->jtype != JT_SPRING) valid = false;
	return Util::to_value(valid);
}

VALUE MSNewton::Spring::create(VALUE self, VALUE v_joint) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_NONE);

	SpringData* cj_data = new SpringData;
	cj_data->min = DEFAULT_MIN;
	cj_data->max = DEFAULT_MAX;
	cj_data->stiff = DEFAULT_STIFF;
	cj_data->damp = DEFAULT_DAMP;
	cj_data->limits_enabled = DEFAULT_LIMITS_ENABLED;
	cj_data->cur_pos = 0.0f;
	cj_data->cur_vel = 0.0f;
	cj_data->cur_accel = 0.0f;
	cj_data->start_pos = DEFAULT_START_POSITION;
	cj_data->controller = DEFAULT_CONTROLLER;
	cj_data->desired_start_pos = cj_data->start_pos * cj_data->controller;
	cj_data->temp_disable_limits = true;

	joint_data->dof = 6;
	joint_data->jtype = JT_SPRING;
	joint_data->cj_data = cj_data;
	joint_data->submit_constraints = submit_constraints;
	joint_data->get_info = get_info;
	joint_data->on_destroy = on_destroy;
	joint_data->on_connect = on_connect;
	joint_data->on_disconnect = on_disconnect;

	return Util::to_value(joint_data);
}

VALUE MSNewton::Spring::get_min(VALUE self, VALUE v_joint) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	WorldData* world_data = (WorldData*)NewtonWorldGetUserData(joint_data->world);
	return Util::to_value(cj_data->min * world_data->inverse_scale);
}

VALUE MSNewton::Spring::set_min(VALUE self, VALUE v_joint, VALUE v_min) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	WorldData* world_data = (WorldData*)NewtonWorldGetUserData(joint_data->world);
	cj_data->min = Util::value_to_dFloat(v_min) * world_data->scale;
	return Util::to_value(cj_data->min * world_data->inverse_scale);
}

VALUE MSNewton::Spring::get_max(VALUE self, VALUE v_joint) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	WorldData* world_data = (WorldData*)NewtonWorldGetUserData(joint_data->world);
	return Util::to_value(cj_data->max * world_data->inverse_scale);
}

VALUE MSNewton::Spring::set_max(VALUE self, VALUE v_joint, VALUE v_max) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	WorldData* world_data = (WorldData*)NewtonWorldGetUserData(joint_data->world);
	cj_data->max = Util::value_to_dFloat(v_max) * world_data->scale;
	return Util::to_value(cj_data->max * world_data->inverse_scale);
}

VALUE MSNewton::Spring::enable_limits(VALUE self, VALUE v_joint, VALUE v_state) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	cj_data->limits_enabled = Util::value_to_bool(v_state);
	return Util::to_value(cj_data->limits_enabled);
}

VALUE MSNewton::Spring::are_limits_enabled(VALUE self, VALUE v_joint) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	return Util::to_value(cj_data->limits_enabled);
}

VALUE MSNewton::Spring::get_stiff(VALUE self, VALUE v_joint) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	return Util::to_value(cj_data->stiff);
}

VALUE MSNewton::Spring::set_stiff(VALUE self, VALUE v_joint, VALUE v_stiff) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	cj_data->stiff = Util::clamp_min<dFloat>(Util::value_to_dFloat(v_stiff), 0.0f);
	return Util::to_value(cj_data->stiff);
}

VALUE MSNewton::Spring::get_damp(VALUE self, VALUE v_joint) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	return Util::to_value(cj_data->damp);
}

VALUE MSNewton::Spring::set_damp(VALUE self, VALUE v_joint, VALUE v_damp) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	cj_data->damp = Util::clamp_min<dFloat>(Util::value_to_dFloat(v_damp), 0.0f);
	return Util::to_value(cj_data->damp);
}

VALUE MSNewton::Spring::get_cur_position(VALUE self, VALUE v_joint) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	WorldData* world_data = (WorldData*)NewtonWorldGetUserData(joint_data->world);
	return Util::to_value((cj_data->cur_pos - cj_data->start_pos * cj_data->controller) * world_data->inverse_scale);
}

VALUE MSNewton::Spring::get_cur_velocity(VALUE self, VALUE v_joint) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	WorldData* world_data = (WorldData*)NewtonWorldGetUserData(joint_data->world);
	return Util::to_value(cj_data->cur_vel * world_data->inverse_scale);
}

VALUE MSNewton::Spring::get_cur_acceleration(VALUE self, VALUE v_joint) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	WorldData* world_data = (WorldData*)NewtonWorldGetUserData(joint_data->world);
	return Util::to_value(cj_data->cur_accel * world_data->inverse_scale);
}

VALUE MSNewton::Spring::get_start_position(VALUE self, VALUE v_joint) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	WorldData* world_data = (WorldData*)NewtonWorldGetUserData(joint_data->world);
	return Util::to_value(cj_data->start_pos * world_data->inverse_scale);
}

VALUE MSNewton::Spring::set_start_position(VALUE self, VALUE v_joint, VALUE v_pos) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	WorldData* world_data = (WorldData*)NewtonWorldGetUserData(joint_data->world);
	cj_data->start_pos = Util::value_to_dFloat(v_pos) * world_data->scale;
	dFloat desired_start_pos = cj_data->start_pos * cj_data->controller;
	if (cj_data->desired_start_pos != desired_start_pos) {
		cj_data->temp_disable_limits = true;
		cj_data->desired_start_pos = desired_start_pos;
		if (joint_data->connected)
			NewtonBodySetSleepState(joint_data->child, 0);
	}
	return Util::to_value(cj_data->start_pos * world_data->inverse_scale);
}

VALUE MSNewton::Spring::get_controller(VALUE self, VALUE v_joint) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	return Util::to_value(cj_data->controller);
}

VALUE MSNewton::Spring::set_controller(VALUE self, VALUE v_joint, VALUE v_controller) {
	JointData* joint_data = Util::value_to_joint2(v_joint, JT_SPRING);
	SpringData* cj_data = (SpringData*)joint_data->cj_data;
	cj_data->controller = Util::value_to_dFloat(v_controller);
	dFloat desired_start_pos = cj_data->start_pos * cj_data->controller;
	if (cj_data->desired_start_pos != desired_start_pos) {
		cj_data->temp_disable_limits = true;
		cj_data->desired_start_pos = desired_start_pos;
		if (joint_data->connected)
			NewtonBodySetSleepState(joint_data->child, 0);
	}
	return Util::to_value(cj_data->controller);
}


void Init_msp_spring(VALUE mNewton) {
	VALUE mSpring = rb_define_module_under(mNewton, "Spring");

	rb_define_module_function(mSpring, "is_valid?", VALUEFUNC(MSNewton::Spring::is_valid), 1);
	rb_define_module_function(mSpring, "create", VALUEFUNC(MSNewton::Spring::create), 1);
	rb_define_module_function(mSpring, "get_min", VALUEFUNC(MSNewton::Spring::get_min), 1);
	rb_define_module_function(mSpring, "set_min", VALUEFUNC(MSNewton::Spring::set_min), 2);
	rb_define_module_function(mSpring, "get_max", VALUEFUNC(MSNewton::Spring::get_max), 1);
	rb_define_module_function(mSpring, "set_max", VALUEFUNC(MSNewton::Spring::set_max), 2);
	rb_define_module_function(mSpring, "enable_limits", VALUEFUNC(MSNewton::Spring::enable_limits), 2);
	rb_define_module_function(mSpring, "are_limits_enabled?", VALUEFUNC(MSNewton::Spring::are_limits_enabled), 1);
	rb_define_module_function(mSpring, "get_stiff", VALUEFUNC(MSNewton::Spring::get_stiff), 1);
	rb_define_module_function(mSpring, "set_stiff", VALUEFUNC(MSNewton::Spring::set_stiff), 2);
	rb_define_module_function(mSpring, "get_damp", VALUEFUNC(MSNewton::Spring::get_damp), 1);
	rb_define_module_function(mSpring, "set_damp", VALUEFUNC(MSNewton::Spring::set_damp), 2);
	rb_define_module_function(mSpring, "get_cur_position", VALUEFUNC(MSNewton::Spring::get_cur_position), 1);
	rb_define_module_function(mSpring, "get_cur_velocity", VALUEFUNC(MSNewton::Spring::get_cur_velocity), 1);
	rb_define_module_function(mSpring, "get_cur_acceleration", VALUEFUNC(MSNewton::Spring::get_cur_acceleration), 1);
	rb_define_module_function(mSpring, "get_start_position", VALUEFUNC(MSNewton::Spring::get_start_position), 1);
	rb_define_module_function(mSpring, "set_start_position", VALUEFUNC(MSNewton::Spring::set_start_position), 2);
	rb_define_module_function(mSpring, "get_controller", VALUEFUNC(MSNewton::Spring::get_controller), 1);
	rb_define_module_function(mSpring, "set_controller", VALUEFUNC(MSNewton::Spring::set_controller), 2);
}