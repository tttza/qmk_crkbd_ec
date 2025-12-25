# Corne EC (tttza) keymapメモ

## 記号ずれを防ぐためのマクロ呼び出し方法

- このキーマップは OS 側を JIS 配列で使う前提です。US キーキャップで記号を正しく出すために `twpair_on_jis` が物理キー入力を US→JIS 変換しています。
- VIA/自作マクロは `process_record_user` を通らないため、通常の `tap_code16()` / `SEND_STRING()` だけだと記号がずれます。
- 対策として、言語フラグ (`CK_EnJIS` / `CK_EnUS`) の状態に応じて US→JIS 変換してから送信するラッパーを用意しています。

### 使い方

- 記号キーを送るときは `tap_code16_lang(KC_AT);` のようにラッパーを使ってください。
- 修飾付きでもそのまま渡せます (例: `tap_code16_lang(LCTL(KC_LBRC));`)。
- フラグ切り替え: `_ADJUST` 層に `CK_EnJIS` (JISに戻す) / `CK_EnUS` (US扱いにする) を配置済みです。

### 追加したヘルパー

- `tap_code16_lang()` : 現在の JIS/US 設定を見て記号キーを変換してから送信するラッパー。
- `us_to_jis_keycode()` : US keycode を JIS 相当へ変換するユーティリティ。必要なら自前の関数から利用できます。
