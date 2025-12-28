# Corne EC (tttza) keymapメモ

## 前提・OS設定

- OS 側を JIS 配列にして使う想定。US キーキャップでも記号位置が合うよう `twpair_on_jis` が物理入力を US→JIS 変換します。
- 言語フラグは EEPROM に保存。`_ADJUST` 層に `CK_EnJIS` (JIS に戻す) / `CK_EnUS` (US 扱いにする) を配置済みです。
- フラグ ON のときは `process_record_user` で記号を変換。VIA/LEADER/自作マクロ経由の送信も下記ラッパー経由で揃えます。

## レイヤー

| Layer   | 役割 |
| --- | --- |
| _QWERTY | ベース。左スペースは Shift タップ、右スペースは Enter タップ。|
| _LOWER  | 数字 / F1–F12 / 各種ブラケット / Backspace / Delete。|
| _RAISE  | 記号、カーソル (←↓↑→ / Home / End / PgUp / PgDn)、`WSEL` を左右に配置。|
| _ADJUST | RGB 設定、言語トグル、Caps Word、handedness swap、SE_UNLK など。|

## 記号ずれ防止の送信方法

- 記号キーは `tap_code16_lang(KC_AT);` のようにラッパー経由で送る。
- 修飾付きもそのまま渡せる (例: `tap_code16_lang(LCTL(KC_LBRC));`)。
- US→JIS 変換だけ欲しい場合は `us_to_jis_keycode()` で手元変換してから送信する。
- `SEND_STRING()` を自作する場合は `send_string_apply_keymap()` をフックさせると文字列も JIS 側に揃えられる。

## テキスト編集系

- `WSEL` (Raise 層右手側): タップで単語選択、Shift を保持したまま押すと行選択。選択解除は `Esc`。
- Caps Word: A–Z と数字・Backspace/Del・ハイフン/アンダーを許可。状態は XIAO ステータス LED (GP12) をシアン点灯。

## Leader (Vim 風ショートカット)

- Leader キー (QK_LEAD) の後に入力。先頭に数字を入れると回数指定 (省略時 1)。
- `y y` で行コピー、`y y p` でコピー後に次行へ貼り付け。
- `d d` または `d` で行削除。
- `y` 単体で行コピー、`p` 単体で貼り付け×回数。
- `g g` でファイル先頭、`g` で末尾移動。
- `u` で Undo、`r` で Redo。
- Leader 待機中はステータス LED がアンバー点灯。

## ビルド/書き込み

- ビルド: `qmk compile -kb sekigon/crkbd_ec -km tttza`
- 書き込み: `qmk flash -kb sekigon/crkbd_ec -km tttza`
