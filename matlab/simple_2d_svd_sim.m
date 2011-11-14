
function simple_2d_svd_sim(delta, target_task, start_phi, residual, thresh=0.01)
	current_phi = start_phi;
	current_task = fk(start_phi);
	while(norm(current_task - target_task) > residual)
	   delta_task = task_step(delta, current_task, target_task);
		
		[U, S, V] = svd (jac(current_phi));
		delta_phi = V * smooth_pinv(S, @smooth_inv, thresh)' * U' * delta_task;
		current_phi += delta_phi;
		
		plot_robot(current_phi);
		sleep(0.01)
		
		current_task = fk(current_phi);
	end
	
end

function inv = smooth_pinv(S, f, thresh)
  N=min(rows(S), columns(S));
  inv = S;
  for i = 1:N
	 inv(i,i) = feval(f, inv(i,i), thresh);
end

function inv = smooth_inv(s, thresh)
  if s < thresh
	 inv = interp1([0, thresh], [0, 1./thresh], s);
  else 
	 inv = 1./s;
  endif
end

function inv = clipped_inv(s, thresh)
  if s < thresh
	 inv = 0;
  else 
	 inv = 1./s;
  endif
end