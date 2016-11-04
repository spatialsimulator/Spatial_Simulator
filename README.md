Spatial Simulator
======================

This program is a PDE simulator for SBML models.
By using this program, users may simulate their spatially extended SBML models.

##How to Compile##
###Dependencies###
+ [libsbml 5.13.0](http://sbml.org/Software/libSBML "libsbml")
+ [gnuplot](http://www.gnuplot.info/ "gnuplot")
+ [Opencv](http://opencv.org/ "OpenCV")

###Build Spatial Simulator###

    % git clone /path/to/spatial_simulator
    % cd Spatial_Simulator
    % make

##How to Use##
###Compile###

    % make

###Run###

    % ./main  -x 100 -y 100 -z 100 -t 10 -d 0.001 -o 500 -c 5 -C 0 -s z30 hoge.xml

  | Options | Definitions|
  |:--------:|:------------|
  |-x | x軸方向のメッシュ数|
  |-y | y軸方向のメッシュ数|
  |-z | z軸方向のメッシュ数|
  |-t | 終了時間|
  |-d | 時間刻み|
  |-o | 何ステップごとに結果出力するか|
  |-c | 結果を画像出力するときのカラーバーのレンジの最高値 (最低値は0としている)|
  |-C | レンジの最低値（少しダサいので今後修正）|
  |-s | どのスライスの何番目を見るかを指定|
  |hoge.xml | モデルが記述されたSBMLファイル|

##To Do##
+ メモリリークちゃんとチェックしてないから大変なことになってそう (すいません)
+ コード汚い(関数でまとめられるところまとめてなかったり) (これもすいません)
+ 単位(Unit)

  特に、現在使っているSBMLファイルはVCellからエクスポートしたものを使っているが、そのファイル内の変数の単位がよくわからない変数をかけてよくわからない補正をしているから
  今はそれを直して使っている(ex.xmlを参照)

+ VCellとy軸の方向が逆だから、VCellからエクスポートしたファイルを使うときは上下逆さになることに注意

      自分のやつ             VCell
          y ^                              - - - -> x
                |                                  |
                |                                  |
                |                                  |
                - - - ->                y  v
                          x

+ AlgebraicRule, FastReaction未実装

+ 陰解法

+ 膜上の拡散を計算するとき、シミュレーション領域におさまらないような反応領域を用いるとセグる。

  膜上拡散アルゴリズムのrefer論文に対処法があるが、現在の実装が不完全

+ 多次元移流の計算をするとき、今はx軸計算->y軸計算->z軸計算みたいに時間分離でやっているが、もっと良い方法があるかも (今の方法でもそれほど精度は悪くないけど)

+ 計算に疎行列を使っているからメモリが大変なことになってしまう


##License##
