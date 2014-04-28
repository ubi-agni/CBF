function ret = test_pinv(q, t, alpha)
	t_cur = fk(q);

	steps = 0;
	accum = round(1/alpha);
	t_buf = zeros(2,accum);

	plot_range = 8;
	do 
		t_last = t_cur;
		delta_t = alpha * (t - t_cur);
		if(norm(delta_t) > alpha)
			delta_t = alpha * delta_t * (1/norm(delta_t));
		end

		j = jac(q);
		j_inv = pinv(j);

		N_p = (diag([1 1 1]) - (j_inv * j));
		q_sub = 0.1 * alpha * (-q);
		delta_q_sub = N_p * q_sub;

		#q = q + inv(pinv(jac(q))*pinv(jac(q)'))*pinv(jac(q)) * (alpha * (t - t_cur));
		#q = q + mm(jac(q), delta_t) * pinv(jac(q)) * delta_t;
		#q = q + (1/abs(det(j_inv)))) * j_inv * delta_t;

		#q_new = q + j_inv * delta_t;
		#norm(q_new - q)/(2*pi)
		#t_new = fk(q_new);
		#n = norm (t_new - (t_cur+delta_t))
		#m = 1/(1+(30 * (n**2)))
		#q = q + m * (q_new - q);

		det_j = (10 * det(j * j')) ** 2;
		mm = det_j / (1 + det_j);
		tr_inv = j' * delta_t;
		delta_q = mm * j_inv * delta_t + (1-mm) * tr_inv * 1/16;
		#q = q + delta_q + mm * delta_q_sub;
		#q = q + j_inv * delta_t;
		[u s v] = svd(j);
		for i = 1:size(s)
			s(i,i) = 1/(s(i,i) + (1/(100*s(i,i)**3)));
		end
		p_inv_damped = v * s' * u';
		q = q + p_inv_damped * delta_t;
		#q = q + j' * delta_t;
		t_cur = fk(q)

		t_buf(:,mod(steps, accum) + 1) = t_cur;

		steps = steps + 1;
		if mod(steps, accum) == 0
			plot([-plot_range plot_range plot_range -plot_range t_buf(1,:) t(1) 0], [-plot_range -plot_range plot_range plot_range t_buf(2,:) t(2) 0], "@");
		end
		sleep(0.01);
		norm(t_last - t_cur)
	#until (norm(t_cur - t) < 0.01);
	until (norm(t_cur - t) < 0.01) || (norm(t_last - t_cur) < 0.001);
	plot([-plot_range plot_range plot_range -plot_range t_buf(1,:) t(1) 0], [-plot_range -plot_range plot_range plot_range t_buf(2,:) t(2) 0], "@");
	ret = q;
end


