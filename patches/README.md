# Patch Bundles

このディレクトリは、upstream/master（commit `acbeec29dab5331fe914f35a53d6b43325881e4d` 時点）からのローカル差分をパッチとしてまとめています。マージ時の衝突回避や再適用に使います。

## パッチ一覧
- patches/common/0004-pico-boot-and-wear-leveling.patch

## 適用方法
```
git apply patches/common/0004-pico-boot-and-wear-leveling.patch
```
パッチが適用済みか不明な場合は `git apply --check <patch>` で検証できます。

## 更新方法
各パッチは該当ファイルの差分から再生成します。
```
# 例: RP2040 boot/wear leveling
git diff upstream/master -- platforms/common.mk platforms/chibios/drivers/wear_leveling/wear_leveling_rp2040_flash.c platforms/chibios/vendors/RP/stage2_bootloaders.c platforms/chibios/mcu_selection.mk > patches/common/0004-pico-boot-and-wear-leveling.patch
```
他のパッチも同様に `git diff upstream/master -- <files>` で取得してください。

