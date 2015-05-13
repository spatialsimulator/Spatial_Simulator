Spatial Simulator
======================

This program is a PDE simulator for SBML models.
By using this program, users may simulate their spatially extended SBML models.

##How to Compile##
###Dependencies###
+ [libsbml 5.11.4](http://sbml.org/Software/libSBML "libsbml")
+ [gnuplot](http://www.gnuplot.info/ "gnuplot")
+ [Opencv](http://opencv.org/ "OpenCV")

###Build Spatial Simulator###

    % git clone https://ii@fun.bio.keio.ac.jp:8443/git/Spatial_Simulator.git
    % cd Spatial_Simulator
    % make

##How to Use##
###Compile###

    % make

###Run###

    % ./main  -x 100 -y 100 -z 100 -t 10 -d 0.001 -o 500 -c 5 -s z30 hoge.xml

  | Options | Definitions|
  |:--------|:------------|  
  |-x | x軸方向のメッシュ数|
  |-y | y軸方向のメッシュ数|
  |-z | z軸方向のメッシュ数|
  |-t | 終了時間|
  |-d | 時間刻み|
  |-o | 何ステップごとに結果出力するか|
  |-c | 結果を画像出力するときのカラーバーのレンジの最高値 (最低値は0としている)|
  |-s | どのスライスの何番目を見るかを指定|
  |hoge.xml | モデルが記述されたSBMLファイル|

##License##
