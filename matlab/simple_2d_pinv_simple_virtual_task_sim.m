
function simple_2d_pinv_simple_virtual_task_sim(delta, target_task, start_phi, residual, weight)
	current_phi = start_phi;
	current_task = fk(start_phi);
	while(norm(current_task - target_task) > residual)
		delta_task = delta * (target_task - current_task);
		
		delta_phi = pinv([jac(current_phi) weight * delta_task]) * delta_task;
		
		current_phi += delta_phi(1:3,:);
		
		plot_robot(current_phi);
		sleep(0.01)
		
		current_task = fk(current_phi);
	end
	
end