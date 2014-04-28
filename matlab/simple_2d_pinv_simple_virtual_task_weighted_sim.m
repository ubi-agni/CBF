
function simple_2d_pinv_simple_virtual_task_weighted_sim(delta, target_task, start_phi, residual, weight_task, weight_anything)
	current_phi = start_phi;
	current_task = fk(start_phi);
	while(norm(current_task - target_task) > residual)
		delta_task = task_step(delta, current_task, target_task);
		
		delta_phi = pinv([jac(current_phi) (weight_task * delta_task) (weight_anything * diag(ones(1,2)))]) * delta_task;
		current_phi += delta_phi(1:3,:);
		
		plot_robot(current_phi);
		sleep(0.01)
		
		current_task = fk(current_phi);
	end
	
end