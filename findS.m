function [S, errs] = findS( U, MaxVol )
	% U : should be a column vector

  
	mu = mean(U);

	initS = randn( size(U) );

	A_base = tril( ones( length(U) ), 0 );

	b_base = MaxVol * ones( length(U), 1 );

	b_base(end) = 0;

	A = [ A_base; -A_base ];

	b = [ b_base; b_base ];

	Aeq = ones(1, length(initS) );

	beq = 0;

	lb  = -U; 

	% if the optimization result is not ideal, try to replace the 'sqp' in the following with 'interior-point'

	options = optimoptions('fmincon','Display','iter','Algorithm','sqp');
    
        [S0, errs0] = fmincon(@objFun, initS, A, b, Aeq, beq, lb, [], [], options);%condition 1
    
	[S, errs] = fmincon(@objFun, initS, A_base, b_base, Aeq, beq, lb, [], [], options);




	%% objFun: the objective function for minimizing.
	function errValue = objFun(S)
		% S : should be a column vector
		
		errValue = (U + S - mu)'*(U + S - mu);

	end
	
end



