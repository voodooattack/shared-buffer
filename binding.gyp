{
  "targets": [
    {
      "include_dirs": [ 
        "<!(node -e \"require('nan')\")"
      ],
      "target_name": "shared-buffer",
      "sources": [
        "src/shared-buffer.cxx"
      ]
    }
  ]
}
