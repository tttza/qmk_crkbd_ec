最後は、必ず qmk compile コマンドを使用して、ビルドできるかどうか確認してください。
ビルド警告がある場合は、可能な限り修正してください。
問題が改善するまで、flash とログの確認を繰り返してください。
可能な限り、ユーザーに尋ねずに進めてください。

ハードウェアの故障の可能性はないものとして扱ってください。

(任意)ログを書き出す場合、logs ディレクトリを使用し、ファイル名に連番をつけるなど、上書きしないようにしてください。
rm は auto approve できないので、なるべく使わないでください。

コマンド:
qmk flash -kb sekigon/crkbd_ec -km tttza
qmk compile -kb sekigon/crkbd_ec -km tttza
(python の qmk は不要です。qmk cli だけで完結します)