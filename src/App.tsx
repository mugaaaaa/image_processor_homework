import { useEffect, useMemo, useRef, useState } from 'react'
import { createTheme, ThemeProvider } from '@mui/material/styles'
import { Box, Card, CardContent, CardHeader, Button, TextField, Typography, Divider } from '@mui/material'
import Grid from '@mui/material/Grid'
import '@fontsource/roboto/400.css'
import '@fontsource/roboto/500.css'

type ImageDataObj = { width: number; height: number; channels: number; data: Uint8Array }

function CanvasImage({ image }: { image?: ImageDataObj }) {
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
  return <canvas ref={canvasRef} style={{ width: '100%', height: 'auto', background: '#222' }} />
}

function App() {
  const theme = useMemo(() => createTheme({ palette: { mode: 'light' } }), [])

  const [current, setCurrent] = useState<ImageDataObj | undefined>(undefined)
  const [uploadPath, setUploadPath] = useState('')
  const [savePathPng, setSavePathPng] = useState('')
  const [savePathPpm, setSavePathPpm] = useState('')

  const [tripSavePath, setTripSavePath] = useState('')
  const [tripLoadPath, setTripLoadPath] = useState('')

  const [resizeW, setResizeW] = useState('256')
  const [resizeH, setResizeH] = useState('256')

  const handleUpload = async () => {
    try {
      if (!uploadPath) return
      const isPpm = uploadPath.endsWith('.ppm')
      const img = isPpm ? await window.native.loadPpm(uploadPath) : await window.native.loadPng(uploadPath)
      setCurrent(img)
    } catch (e) {
      console.error(e)
      alert('加载图片失败: ' + (e as any).message)
    }
  }

  const handleSavePng = async () => {
    try {
      if (!current || !savePathPng) return
      const ok = await window.native.savePng(savePathPng, current)
      alert(ok ? '保存 PNG 成功' : '保存 PNG 失败')
    } catch (e) { alert('保存 PNG 出错: ' + (e as any).message) }
  }

  const handleSavePpm = async () => {
    try {
      if (!current || !savePathPpm) return
      const ok = await window.native.savePpm(savePathPpm, current)
      alert(ok ? '保存 PPM 成功' : '保存 PPM 失败')
    } catch (e) { alert('保存 PPM 出错: ' + (e as any).message) }
  }

  const handleCompressSave = async () => {
    try {
      if (!current || !tripSavePath) return
      const ok = await window.native.compressorSave(tripSavePath, current)
      alert(ok ? '保存 .trip 成功' : '保存 .trip 失败')
    } catch (e) { alert('保存 .trip 出错: ' + (e as any).message) }
  }

  const handleLoadTrip = async () => {
    try {
      if (!tripLoadPath) return
      const img = await window.native.compressorLoad(tripLoadPath)
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
        <Box sx={{ flex: 1, p: 2 }}>
          <Typography variant="h6" gutterBottom>图像预览</Typography>
          <Divider sx={{ mb: 2 }} />
          <CanvasImage image={current} />
        </Box>

        {/* 右侧操作区域 */}
        <Box sx={{ width: 420, p: 2, borderLeft: '1px solid #eee', overflow: 'auto' }}>
          <Grid container spacing={2}>
            {/* Card 1: 上传/保存 */}
            <Grid item xs={12}>
              <Card>
                <CardHeader title="上传图片与保存图片" />
                <CardContent>
                  <TextField fullWidth label="图片路径 (.png/.ppm)" value={uploadPath} onChange={e => setUploadPath(e.target.value)} sx={{ mb: 1 }} />
                  <Button variant="contained" onClick={handleUpload}>加载图片</Button>
                  <Divider sx={{ my: 2 }} />
                  <TextField fullWidth label="保存 PNG 路径" value={savePathPng} onChange={e => setSavePathPng(e.target.value)} sx={{ mb: 1 }} />
                  <Button variant="outlined" onClick={handleSavePng} disabled={!current}>保存 PNG</Button>
                  <Divider sx={{ my: 2 }} />
                  <TextField fullWidth label="保存 PPM 路径" value={savePathPpm} onChange={e => setSavePathPpm(e.target.value)} sx={{ mb: 1 }} />
                  <Button variant="outlined" onClick={handleSavePpm} disabled={!current}>保存 PPM</Button>
                </CardContent>
              </Card>
            </Grid>

            {/* Card 2: 压缩/解压 .trip */}
            <Grid item xs={12}>
              <Card>
                <CardHeader title="压缩成三元组并保存 / 读入解码" />
                <CardContent>
                  <TextField fullWidth label="保存 .trip 路径" value={tripSavePath} onChange={e => setTripSavePath(e.target.value)} sx={{ mb: 1 }} />
                  <Button variant="contained" onClick={handleCompressSave} disabled={!current}>保存 .trip</Button>
                  <Divider sx={{ my: 2 }} />
                  <TextField fullWidth label="读取 .trip 路径" value={tripLoadPath} onChange={e => setTripLoadPath(e.target.value)} sx={{ mb: 1 }} />
                  <Button variant="outlined" onClick={handleLoadTrip}>读取并解码</Button>
                </CardContent>
              </Card>
            </Grid>

            {/* Card 3: 彩色转灰度 + 缩放 */}
            <Grid item xs={12}>
              <Card>
                <CardHeader title="彩色转灰度 / 图片缩放" />
                <CardContent>
                  <Button variant="contained" onClick={handleToGray} disabled={!current}>彩色转灰度</Button>
                  <Divider sx={{ my: 2 }} />
                  <Box sx={{ display: 'flex', gap: 1 }}>
                    <TextField label="宽" value={resizeW} onChange={e => setResizeW(e.target.value)} sx={{ flex: 1 }} />
                    <TextField label="高" value={resizeH} onChange={e => setResizeH(e.target.value)} sx={{ flex: 1 }} />
                  </Box>
                  <Button sx={{ mt: 1 }} variant="outlined" onClick={handleResize} disabled={!current}>缩放</Button>
                </CardContent>
              </Card>
            </Grid>
          </Grid>
        </Box>
      </Box>
    </ThemeProvider>
  )
}

export default App
