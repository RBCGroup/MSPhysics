module MSPhysics

  # @since 1.0.0
  class CurvyPiston < Joint

    DEFAULT_ANGULAR_FRICTION = 0.0
    DEFAULT_RATE = 4.0
    DEFAULT_POWER = 0.0
    DEFAULT_ALIGNMENT_POWER = 0.0
    DEFAULT_REDUCTION_RATIO = 0.1
    DEFAULT_CONTROLLER = nil
    DEFAULT_CONTROLLER_MODE = 0
    DEFAULT_LOOP_ENABLED = false
    DEFAULT_ALIGNMENT_ENABLED = true
    DEFAULT_ROTATION_ENABLED = true

    # Create a CurvyPiston joint.
    # @param [MSPhysics::World] world
    # @param [MSPhysics::Body, nil] parent
    # @param [Geom::Transformation, Array<Numeric>] pin_tra Pin transformation
    #   in global space. Matrix origin is interpreted as the pin position.
    #   Matrix Z-axis is interpreted as the pin direction.
    # @param [Sketchup::Group, Sketchup::ComponentInstance, nil] group
    def initialize(world, parent, pin_tra, group = nil)
      super(world, parent, pin_tra, group)
      MSPhysics::Newton::CurvyPiston.create(@address)
      MSPhysics::Newton::CurvyPiston.set_angular_friction(@address, DEFAULT_ANGULAR_FRICTION)
      MSPhysics::Newton::CurvyPiston.set_rate(@address, DEFAULT_RATE)
      MSPhysics::Newton::CurvyPiston.set_power(@address, DEFAULT_POWER)
      MSPhysics::Newton::CurvyPiston.set_alignment_power(@address, DEFAULT_ALIGNMENT_POWER)
      MSPhysics::Newton::CurvyPiston.set_reduction_ratio(@address, DEFAULT_REDUCTION_RATIO)
      MSPhysics::Newton::CurvyPiston.set_controller(@address, DEFAULT_CONTROLLER)
      MSPhysics::Newton::CurvyPiston.set_controller_mode(@address, DEFAULT_CONTROLLER_MODE)
      MSPhysics::Newton::CurvyPiston.enable_loop(@address, DEFAULT_LOOP_ENABLED)
      MSPhysics::Newton::CurvyPiston.enable_alignment(@address, DEFAULT_ALIGNMENT_ENABLED)
      MSPhysics::Newton::CurvyPiston.enable_rotation(@address, DEFAULT_ROTATION_ENABLED)
    end

    # Append point to the curve.
    # @param [Geom::Point3d] position
    # @return [Integer] point index.
    def add_point(position)
      MSPhysics::Newton::CurvyPiston.add_point(@address, position)
    end

    # Remove point from curve at index.
    # @param [Integer] index
    # @return [Boolean] success
    def remove_point(index)
      MSPhysics::Newton::CurvyPiston.remove_point(@address, index)
    end

    # Get all points the curve is associated of.
    # @return [Array<Geom::Point3d>]
    def points
      MSPhysics::Newton::CurvyPiston.get_points(@address)
    end

    # Get the number of points that make up the curve.
    # @return [Integer]
    def points_size
      MSPhysics::Newton::CurvyPiston.get_points_size(@address)
    end

    # Remove all points that make up the curve.
    # @return [Integer] The number of points removed.
    def clear
      MSPhysics::Newton::CurvyPiston.clear_points(@address)
    end

    # Get curve length in meters.
    # @return [Numeric]
    def length
      MSPhysics::Newton::CurvyPiston.get_length(@address)
    end

    # Get point position by index.
    # @param [Integer] index
    # @return [Geom::Point3d, nil] Point position in global space if the index
    #   references an existing point; +nil+ otherwise.
    def get_point_position(index)
      MSPhysics::Newton::CurvyPiston.get_point_position(@address, index)
    end

    # Set point position by index.
    # @param [Integer] index
    # @param [Geom::Point3d] position Point position in global space.
    # @return [Boolean] success
    def set_point_position(index, position)
      MSPhysics::Newton::CurvyPiston.set_point_position(@address, index, position)
    end

    # Get current position along the curve in meters.
    # @return [Numeric]
    def cur_position
      MSPhysics::Newton::CurvyPiston.get_cur_position(@address)
    end

    # Get current velocity along the curve in meters per second.
    # @return [Numeric]
    def cur_velocity
      MSPhysics::Newton::CurvyPiston.get_cur_velocity(@address)
    end

    # Get current acceleration along the curve in meters per second per second.
    # @return [Numeric]
    def cur_acceleration
      MSPhysics::Newton::CurvyPiston.get_cur_acceleration(@address)
    end

    # Get current point on the curve.
    # @return [Geom::Point3d, nil] A point in global space or +nil+ if curve is
    #   empty or joint is disconnected.
    def cur_point
      MSPhysics::Newton::CurvyPiston.get_cur_point(@address)
    end

    # Get current vector on the curve.
    # @return [Geom::Vector3d, nil] A vector in global space or +nil+ if curve
    #   is empty or joint is disconnected.
    def cur_vector
      MSPhysics::Newton::CurvyPiston.get_cur_vector(@address)
    end

    # Get current normal matrix on the curve.
    # @return [Geom::Transformation, nil] A transformation in global space or
    #   +nil+ if curve is empty or joint is disconnected.
    def cur_normal_matrix
      MSPhysics::Newton::CurvyPiston.get_cur_normal_matrix(@address)
    end

    # Get rotational friction.
    # @return [Numeric] A numeric value greater than or equal to zero.
    def angular_friction
      MSPhysics::Newton::CurvyPiston.get_angular_friction(@address)
    end

    # Set rotational friction.
    # @param [Numeric] value A numeric value greater than or equal to zero.
    def angular_friction=(value)
      MSPhysics::Newton::CurvyPiston.set_angular_friction(@address, value)
    end

    # Get maximum linear rate in meters per second.
    # @return [Numeric] A value greater than or equal to zero.
    def rate
      MSPhysics::Newton::CurvyPiston.get_rate(@address)
    end

    # Set maximum linear rate in meters per second.
    # @param [Numeric] value A value greater than or equal to zero.
    def rate=(value)
      MSPhysics::Newton::CurvyPiston.set_rate(@address, value)
    end

    # Get movement power in Watts.
    # @note A power value of zero represents maximum power.
    # @return [Numeric] A value greater than or equal to zero.
    def power
      MSPhysics::Newton::CurvyPiston.get_power(@address)
    end

    # Set movement power in Watts.
    # @note A power value of zero represents maximum power.
    # @param [Numeric] value A value greater than or equal to zero.
    def power=(value)
      MSPhysics::Newton::CurvyPiston.set_power(@address, value)
    end

    # Get linear reduction ratio.
    # @note Reduction ratio is a feature that reduces linear rate of the joint
    #   when its current position nears its desired position. Linear reduction
    #   ratio starts acting upon the linear rate of the joint when the
    #   difference between the current position and the desired position of the
    #   joint is less than <tt>rate * reduction_ratio</tt> meters.
    # @note A reduction ratio of zero disables the reduction feature.
    # @note A typical reduction ratio value is 0.1.
    # @return [Numeric] A value between 0.0 and 1.0.
    def reduction_ratio
      MSPhysics::Newton::CurvyPiston.get_reduction_ratio(@address)
    end

    # Get linear reduction ratio.
    # @note Reduction ratio is a feature that reduces linear rate of the joint
    #   when its current position nears its desired position. Linear reduction
    #   ratio starts acting upon the linear rate of the joint when the
    #   difference between the current position and the desired position of the
    #   joint is less than <tt>rate * reduction_ratio</tt> meters.
    # @note A reduction ratio of zero disables the reduction feature.
    # @note A typical reduction ratio value is 0.1.
    # @param [Numeric] value A value between 0.0 and 1.0.
    def reduction_ratio=(value)
      MSPhysics::Newton::CurvyPiston.set_reduction_ratio(@address, value)
    end

    # Get curvy piston controller.
    # @return [Numeric, nil] Returns one of the following values:
    #   * Desired position in meters if the controller mode is 0.
    #   * Magnitude and direction of the linear rate if controller mode is 1.
    #   * +nil+ if piston is turned off.
    def controller
      MSPhysics::Newton::CurvyPiston.get_controller(@address)
    end

    # Set curvy piston controller.
    # @param [Numeric, nil] value Accepts one of the following values:
    #   * Desired position in meters if the controller mode is 0.
    #   * Magnitude and direction of the linear rate if controller mode is 1.
    #   * +nil+ to turn off the piston.
    def controller=(value)
      MSPhysics::Newton::CurvyPiston.set_controller(@address, value)
    end

    # Get controller mode.
    # @return [Integer] Returns one of the following values:
    #   * 0 to control curvy piston by position.
    #   * 1 to control curvy piston by speed - strict.
    #   * 2 to control curvy piston by speed - adaptive.
    def controller_mode
      MSPhysics::Newton::CurvyPiston.get_controller_mode(@address)
    end

    # Set controller mode.
    # @param [Integer] mode Pass one of the following values:
    #   * 0 to control curvy piston by position.
    #   * 1 to control curvy piston by speed - strict.
    #   * 2 to control curvy piston by speed - adaptive.
    def controller_mode=(mode)
      MSPhysics::Newton::CurvyPiston.set_controller_mode(@address, mode)
    end

    # Determine whether curve looping is enabled.
    # @return [Boolean]
    def loop_enabled?
      MSPhysics::Newton::CurvyPiston.loop_enabled?(@address)
    end

    # Enable/disable curve looping.
    # @param [Boolean] state
    def loop_enabled=(state)
      MSPhysics::Newton::CurvyPiston.enable_loop(@address, state)
    end

    # Determine whether the connected body is supposed to align with the
    # direction of curve.
    # @return [Boolean]
    def alignment_enabled?
      MSPhysics::Newton::CurvyPiston.alignment_enabled?(@address)
    end

    # Enable/disable alignment to curve.
    # @param [Boolean] state
    def alignment_enabled=(state)
      MSPhysics::Newton::CurvyPiston.enable_alignment(@address, state)
    end

    # Get alignment power.
    # @note Has an effect only if alignment is enabled.
    # @return [Numeric] A value greater than or equal to zero.
    def alignment_power
      MSPhysics::Newton::CurvyPiston.get_alignment_power(@address)
    end

    # Set alignment power.
    # @note Has an effect only if alignment is enabled.
    # @param [Numeric] value A value greater than or equal to zero. Pass zero to
    #   use maximum power.
    def alignment_power=(value)
      MSPhysics::Newton::CurvyPiston.set_alignment_power(@address, value)
    end

    # Determine whether the rotation along the current point on curve is
    # enabled.
    # @return [Boolean]
    def rotation_enabled?
      MSPhysics::Newton::CurvyPiston.rotation_enabled?(@address)
    end

    # Enable/disable rotation along the current point on curve.
    # @param [Boolean] state
    def rotation_enabled=(state)
      MSPhysics::Newton::CurvyPiston.enable_rotation(@address, state)
    end

    # Get normal matrix corresponding to a particular distance on the curve.
    # @param [Numeric] distance Linear position on curve in meters.
    # @return [Geom::Transformation, nil] A transformation in global space or
    #   +nil+ if curve is empty or joint is disconnected.
    def normal_matrix_at_position(distance)
      MSPhysics::Newton::CurvyPiston.get_normal_martix_at_position(@address, distance)
    end

    # Get normal matrix corresponding to a particular point on or beside the
    # curve.
    # @param [Geom::Point3d] point A point in global space.
    # @return [Array<(Geom::Transformation, Numeric)>, nil] A transformation in
    #   global space along with overpass value or +nil+ if curve is empty or
    #   joint is disconnected.
    def normal_matrix_at_point(point)
      MSPhysics::Newton::CurvyPiston.get_normal_martix_at_point(@address, point)
    end

    # Get all normal matrices of the curve.
    # @return [Array<Geom::Transformation>] An array of the normal matrices.
    def normal_matrices
      MSPhysics::Newton::CurvyPiston.get_normal_matrices(@address)
    end

  end # class CurvyPiston < Joint
end # module MSPhysics
