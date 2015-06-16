String to MathML
=================
This program creates a MathML snippet from a standard input.

##Dependencies##
+ python 2.7
+ [sympy](http://docs.sympy.org/dev/index.html "sympy")

##Installation##
    % easy_install pip
    % pip install sympy

##Example##
###Create MathML snippet from string###
    % python stringtoml.py
      Variables x,y,z
      Parameters j,k,l

      Enter equation
      sqrt(1/x**2)
      <apply>
        <root/>
        <apply>
          <power/>
          <ci>x</ci>
          <cn>-2</cn>
        </apply>
      </apply>

###create circle/sphere domains###
    % python circle.py
      Enter radius
      5
      area
      <apply>
        <times/>
        <cn>25</cn>
        <pi/>
      </apply>

      volume
      <apply>
        <divide/>
        <apply>
          <times/>
          <cn>500</cn>
          <pi/>
          </apply>
          <cn>3</cn>
      </apply>
