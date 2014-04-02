# project problem

recursion:

$$
v_{i,j} = b_j(i) \sum_{k=0}^nv_{i-1,k}a_{k,j}
$$

expansion:

$$\begin{align}
v_0 & = [v_{0,0} , & v_{0,1}] \\\\
v_1 & = [b_0(1)(a_{0,0}v_{0,0} + a_{1,0}v_{0,1}) , & b_1(1)(a_{0,1}v_{0,0} + a_{1,1}v_{0,1})] \\\\
    & = [a_{0,0}b_0(1)v_{0,0} + a_{1,0}b_0(1)v_{0,1} , & a_{0,1}b_1(1)v_{0,0} + a_{1,1}b_1(1)v_{0,1}] \\\\
v_2 & = [b_0(2)(b_0(1)(a_{0,0}v_{0,0} + a_{1,0}v_{0,1})a_{0,0} + b_1(1)(a_{0,1}v_{0,0} + a_{1,1}v_{0,1})a_{1,0}) , & b_1(2)(b_0(1)(a_{0,0}v_{0,0} + a_{1,0}v_{0,1})a_{0,1} + b_1(1)(a_{0,1}v_{0,0} + a_{1,1}v_{0,1})a_{1,1})] \\\\
    & = [a_{0,0}^2b_0(1)b_0(2)v_{0,0} + a_{0,0}a_{1,0}b_0(1)b_0(2)v_{0,1} + a_{0,1}a_{1,0}b_0(2)b_1(1)v_{0,0}, a_{1,0}a_{1,1}b_0(2)b_1(1)v_{0,1} , & \dots]
\end{align}$$

# homework problem

recursion:

$$
x_i = a_ix_{i-1} + b_i
$$

expansion:

$$\begin{align}
x_0 & = x_0 \\\\
x_1 & = a_1x_0 + b_1 \\\\
x_2 & = a_2a_1x_0 + a_2b_1 + b_2 \\\\
x_3 & = a_3a_2a_1x_0 + a_3a_2b_1 + a_3b_2 + b_3 \\\\
x_4 & = a_4a_3a_2a_1x_0 + a_4a_3a_2b_1 + a_4a_3b_2 + a_4b_3 + b_4 \\\\
\end{align}$$

If we define $c_i$ as $x_0$ times the prefix product of the $a_i$ values
(letting $a_0 = 1$), we get:

$$\begin{align}
x_0 & = c_0 \\\\
x_1 & = c_1 + b_1 \\\\
x_2 & = c_2 + \frac{c_2}{c_1}b_1 + b_2 \\\\
x_3 & = c_3 + \frac{c_3}{c_1}b_1 + \frac{c_3}{c_2}b_2 + b_3 \\\\
x_4 & = c_4 + \frac{c_4}{c_1}b_1 + \frac{c_4}{c_2}b_2 + \frac{c_4}{c_3}b_3 + b_4 \\\\
\end{align}$$

And factor out the $c_i$ value:

$$\begin{align}
x_0 & = c_0\left(\frac{1}{1}\right) \\\\
x_1 & = c_1\left(\frac{1}{1} + \frac{b_1}{c_1}\right) \\\\
x_2 & = c_2\left(\frac{1}{1} + \frac{b_1}{c_1} + \frac{b_2}{c_2}\right) \\\\
x_3 & = c_3\left(\frac{1}{1} + \frac{b_1}{c_1} + \frac{b_2}{c_2} + \frac{b_3}{c_3}\right) \\\\
x_4 & = c_4\left(\frac{1}{1} + \frac{b_1}{c_1} + \frac{b_2}{c_2} + \frac{b_3}{c_3} + \frac{b_4}{c_4}\right) \\\\
\end{align}$$

So it's clear that if we define $d_i = \frac{b_i}{c_i}$, and set $e_i$ to the
prefix sum of $d_i$, we can accomplish this by doing two scans (one for $c_i$
and one for $e_i$), and then setting $x_i = c_id_i$. Work-depth pseudocode:

      function operation(a, b, n)
      % assume 'multiply' and 'add' are associative operations and 'scan' is a
      % parallel scan function like the one from class
      c = scan(a, multiply)
      parfor i=0:n-1
        d(i) = b(i) / c(i)
      end
      e = scan(d, add)
      parfor i=0:n-1
        x(i) = c(i) * e(i)
      end
      return e

