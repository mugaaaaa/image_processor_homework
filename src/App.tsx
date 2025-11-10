import { useEffect, useMemo, useRef, useState } from 'react'
import { createTheme, ThemeProvider } from '@mui/material/styles'
import { Box, Card, CardContent, CardHeader, Button, Typography, Divider, TextField } from '@mui/material'
import '@fontsource/roboto/400.css'
import '@fontsource/roboto/500.css'

type ImageDataObj = { width: number; height: number; channels: number; data: Uint8Array }

function CanvasImage({ image, fixedWidth, fixedHeight }: { image?: ImageDataObj, fixedWidth: number, fixedHeight: number }) {
  const canvasRef = useRef<HTMLCanvasElement>(null)
  useEffect(() => {
    const canvas = canvasRef.current
    if (!canvas || !image) return
    canvas.width = image.width
    canvas.height = image.height
    const ctx = canvas.getContext('2d')!
    const imgData = ctx.createImageData(image.width, image.height)
    const src = image.data
    const dst = imgData.data
    if (image.channels === 1) {
      for (let i = 0, j = 0; i < src.length; i++, j += 4) {
        const g = src[i]
        dst[j] = g; dst[j+1] = g; dst[j+2] = g; dst[j+3] = 255
      }
    } else {
      // src is BGR; convert to RGBA
      for (let i = 0, j = 0; i < src.length; i += 3, j += 4) {
        const b = src[i], g = src[i+1], r = src[i+2]
        dst[j] = r; dst[j+1] = g; dst[j+2] = b; dst[j+3] = 255
      }
    }
    ctx.putImageData(imgData, 0, 0)
  }, [image])
  return <canvas ref={canvasRef} style={{ width: fixedWidth, height: fixedHeight, background: '#222' }} />
}

function App() {
  const theme = useMemo(() => createTheme({ palette: { mode: 'light' } }), [])

  const [current, setCurrent] = useState<ImageDataObj | undefined>(undefined)
  // fixed layout sizes
  const LEFT_WIDTH = 540
  const LEFT_HEIGHT = 540

  // 不使用默认值，初始为空字符串，让用户显式输入
  const [resizeW, setResizeW] = useState('')
  const [resizeH, setResizeH] = useState('')

  const handleUpload = async () => {
    try {
      const chosen = await window.native.openImageDialog()
      if (!chosen) return
      const isPpm = chosen.endsWith('.ppm')
      const img = isPpm ? await window.native.loadPpm(chosen) : await window.native.loadPng(chosen)
      setCurrent(img)
    } catch (e) {
      console.error(e)
      alert('加载图片失败: ' + (e as any).message)
    }
  }

  const handleSavePng = async () => {
    try {
      if (!current) return
      const target = await window.native.savePngDialog()
      if (!target) return
      const ok = await window.native.savePng(target, current)
      alert(ok ? '保存 PNG 成功' : '保存 PNG 失败')
    } catch (e) { alert('保存 PNG 出错: ' + (e as any).message) }
  }

  const handleSavePpm = async () => {
    try {
      if (!current) return
      const target = await window.native.savePpmDialog()
      if (!target) return
      const ok = await window.native.savePpm(target, current)
      alert(ok ? '保存 PPM 成功' : '保存 PPM 失败')
    } catch (e) { alert('保存 PPM 出错: ' + (e as any).message) }
  }

  const handleCompressSave = async () => {
    try {
      if (!current) return
      const target = await window.native.saveTripDialog()
      if (!target) return
      const ok = await window.native.compressorSave(target, current)
      alert(ok ? '保存 .trip 成功' : '保存 .trip 失败')
    } catch (e) { alert('保存 .trip 出错: ' + (e as any).message) }
  }

  const handleLoadTrip = async () => {
    try {
      const chosen = await window.native.openTripDialog()
      if (!chosen) return
      const img = await window.native.compressorLoad(chosen)
      setCurrent(img)
    } catch (e) { alert('读入 .trip 出错: ' + (e as any).message) }
  }

  const handleToGray = async () => {
    try {
      if (!current) return
      if (current.channels !== 3) { alert('当前不是彩色图'); return }
      const img = await window.native.toGray({ width: current.width, height: current.height, data: current.data })
      setCurrent(img)
    } catch (e) { alert('灰度转换出错: ' + (e as any).message) }
  }

  const handleResize = async () => {
    try {
      if (!current) return
      const w = parseInt(resizeW, 10), h = parseInt(resizeH, 10)
      if (!Number.isFinite(w) || !Number.isFinite(h) || w <= 0 || h <= 0) { alert('尺寸不合法'); return }
      const img = await window.native.resize(current, w, h)
      setCurrent(img)
    } catch (e) { alert('缩放出错: ' + (e as any).message) }
  }

  return (
    <ThemeProvider theme={theme}>
      <Box sx={{ height: '100vh', display: 'flex' }}>
        {/* 左侧图片显示区域 */}
        <Box sx={{ width: LEFT_WIDTH, p: 2, display: 'flex', flexDirection: 'column' }}>
          <Typography variant="h6" gutterBottom>
            图像预览{current ? `（${current.width}×${current.height}）` : ''}
          </Typography>
          <Divider sx={{ mb: 2 }} />
          <Box sx={{ flex: 1, width: LEFT_WIDTH, height: LEFT_HEIGHT, border: '1px solid #eee', display: 'flex', alignItems: 'center', justifyContent: 'center', overflow: 'hidden' }}>
            {/* 固定显示区域，不随图片大小变化；通过 CSS 控制画布尺寸 */}
            <CanvasImage image={current} fixedWidth={LEFT_WIDTH} fixedHeight={LEFT_HEIGHT} />
          </Box>
        </Box>

        {/* 右侧操作区域（三卡布局） */}
        <Box sx={{ flex: 1, p: 2, borderLeft: '1px solid #eee', overflow: 'hidden', display: 'flex', flexDirection: 'column', gap: 2, height: LEFT_HEIGHT }}>
          {/* Card 1: 图片读写 */}
          <Card sx={{ flex: 1 }}>
            <CardHeader title="图片读写" titleTypographyProps={{ sx: { fontSize: '1rem' } }} />
            <CardContent sx={{
              '& .MuiButton-root': { fontSize: '0.85rem', textTransform: 'none' },
              '& .MuiTypography-root': { fontSize: '0.9rem' },
              '& .MuiInputBase-input': { fontSize: '0.85rem' },
              '& .MuiInputLabel-root': { fontSize: '0.8rem' }
            }}>
              <Box sx={{ display: 'flex', gap: 1, flexWrap: 'wrap' }}>
                <Button variant="contained" onClick={handleUpload}>读入图片</Button>
                <Button variant="outlined" onClick={handleSavePng} disabled={!current}>保存为 .png</Button>
                <Button variant="outlined" onClick={handleSavePpm} disabled={!current}>保存为 .ppm</Button>
              </Box>
            </CardContent>
          </Card>

          {/* Card 2: 图片压缩 */}
          <Card sx={{ flex: '0 0 160px' }}>
            <CardHeader title="图片压缩" titleTypographyProps={{ sx: { fontSize: '1rem' } }} />
            <CardContent sx={{ '& .MuiButton-root': { fontSize: '0.85rem', textTransform: 'none' } }}>
              <Box sx={{ display: 'flex', gap: 1, flexWrap: 'wrap' }}>
                <Button variant="contained" onClick={handleLoadTrip}>读取.trip并解压</Button>
                <Button variant="outlined" onClick={handleCompressSave} disabled={!current}>压缩并保存为.trip</Button>
              </Box>
            </CardContent>
          </Card>

          {/* Card 3: 图片处理 */}
          <Card sx={{ height: 200 }}>
            <CardHeader title="图片处理" titleTypographyProps={{ sx: { fontSize: '1rem' } }} />
            <CardContent sx={{
              '& .MuiButton-root': { fontSize: '0.85rem', textTransform: 'none' },
              '& .MuiInputBase-input': { fontSize: '0.85rem' },
              '& .MuiInputLabel-root': { fontSize: '0.8rem' }
            }}>
              <Box sx={{ display: 'flex', gap: 1 }}>
                <TextField label="宽度" value={resizeW} onChange={e => setResizeW(e.target.value)} sx={{ flex: 1 }} />
                <TextField label="高度" value={resizeH} onChange={e => setResizeH(e.target.value)} sx={{ flex: 1 }} />
              </Box>
              <Box sx={{ display: 'flex', gap: 1, mt: 2 }}>
                <Button variant="contained" onClick={handleResize} disabled={!current}>缩放</Button>
                <Button variant="contained" onClick={handleToGray} disabled={!current}>灰度化</Button>
              </Box>
            </CardContent>
          </Card>
        </Box>
      </Box>
    </ThemeProvider>
  )
}

export default App
